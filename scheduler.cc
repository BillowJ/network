#include "scheduler.h"
#include "macro.h"
#include "hook.h"

namespace Global
{

static thread_local Fiber* t_scheduler_fiber = nullptr;
static thread_local Scheduler* t_scheduler = nullptr;


Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name)
{
    assert(threads > 0);
    if(use_caller)
    {
        Global::Fiber::GetThis();
        --threads;
        assert(GetThis() == nullptr);   // 目前线程在初始化前不存在Scheduler
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        Global::Thread::SetName(m_name);
        t_scheduler_fiber = m_rootFiber.get();
        m_rootThread = Global::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    }
    else
    {
         m_rootThread = -1;         // 任意线程
    }
    m_threadCount = threads;
}

Scheduler::~Scheduler()
{
    assert(m_stopping);
    if(GetThis() == this)
    {
        t_scheduler = nullptr;
    }
}

Scheduler* Scheduler::GetThis()
{
    return t_scheduler;
}

void Scheduler::setThis() {
    t_scheduler = this;
}

Fiber* Scheduler::GetMainFiber()
{
    return t_scheduler_fiber;
}

void Scheduler::start()
{
    MutexType::LockGuard Lock(m_mutex);
    if(!m_stopping)
    {
        return;
    }
    m_stopping = false;
    assert(m_threads.empty());

    m_threads.resize(m_threadCount);
    for(size_t i = 0; i < m_threadCount; i++)
    {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this)
                            , m_name+"_"+std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    Lock.unlock();
}

void Scheduler::stop()
{
    m_autoStop = true;
    if(m_rootFiber 
            && 0 == m_threadCount 
            && (m_rootFiber->getState() == Fiber::TERM
                || m_rootFiber->getState() == Fiber::INIT))
    {
        std::cout << "stop" << std::endl;
        m_stopping = true;
        if(stopping())
        {
            return;
        }
    }
    if(m_rootThread != -1)
    {
        assert(GetThis() == this);
    }
    else
    {
        assert(GetThis() != this);
    }
    m_stopping = true;

    for(size_t i = 0; i < m_threadCount; i++)
    {
        tickle();   // 唤醒线程自行结束
    }
    if(m_rootFiber)
    {
        tickle();
    }
    if(m_rootFiber)
    {
        if(!stopping())         // 不符合退出条件
        {
            m_rootFiber->call();
        }
    }

    std::vector<Thread::ptr> thrs;
    {
        MutexType::LockGuard Lock(m_mutex);
        thrs.swap(m_threads);
    }
    for(auto& i : thrs)
    {
        i->join();
    }
}

void Scheduler::run()
{
    std::cout << m_name << "run" << std::endl;
    setThis();      // 多线程设置thread_local
    set_hook_enable(true);
    if(m_rootThread != Global::GetThreadId())
    {
        t_scheduler_fiber = Fiber::GetThis().get();     //Fiber里的threadFiber
    }
    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;
    FiberAndThread ft;

    while(true)
    {
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::LockGuard Lock(m_mutex);
            auto it = m_fibers.begin();
            while(it != m_fibers.end())
            {
                // 指定thread挂载的fiber或者cb
                if(it->thread != -1 && it->thread != Global::GetThreadId())
                {
                    ++it;
                    tickle_me = true;   // 发出信号通知其他线程处理
                    continue;
                }
                assert(it->cb || it->fiber);
                if(it->fiber && it->fiber->getState() == Fiber::EXEC)
                {
                    ++it;
                    continue;
                }
                ft = *it;
                m_fibers.erase(it++);
                ++m_activeThreadCount;  // 拿到直接工作线程+1 防止idle提前结束
                is_active = true;   //当前线程是否激活
                break;
            }
            tickle_me |= (it != m_fibers.end());
        }
        if(tickle_me)
        {
            tickle();
        }
        if(ft.fiber && (ft.fiber->getState() != Fiber::TERM
                    && ft.fiber->getState() != Fiber::EXCEPT))
        {
            ft.fiber->swapIn();
            --m_activeThreadCount;
            if(ft.fiber->getState() == Fiber::READY)
            {
                schedule(ft.fiber);
            }
            else if(ft.fiber->getState() != Fiber::TERM
                    && ft.fiber->getState() != Fiber::EXCEPT)
            {
                ft.fiber->m_state = Fiber::HOLD;
            }
            ft.reset();
        }
        else if(ft.cb)
        {
            // 创建一个协程用于执行回调
            if(cb_fiber) cb_fiber->reset(ft.cb);
            else cb_fiber.reset(new Fiber(ft.cb));
            ft.reset();
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if(cb_fiber->getState() == Fiber::READY)
            {
                schedule(cb_fiber);
                cb_fiber.reset();
            }
            else if(cb_fiber->getState() == Fiber::EXCEPT
                 || cb_fiber->getState() == Fiber::TERM)
            {
                cb_fiber->reset(nullptr);
            }
            else
            {
                cb_fiber->m_state = Fiber::HOLD;
                cb_fiber.reset();
            }
        }
        else
        {
            if(is_active)   // 两种情况都不满足但线程被激活
            {
                --m_activeThreadCount;
                continue;
            }
            if(idle_fiber->getState() == Fiber::TERM)
            {
                std::cout << "idle fiber term" << std::endl;
                break;
            }
            ++m_idleThreadCount;
            idle_fiber->swapIn();       // 执行idle协程
            --m_idleThreadCount;
            if(idle_fiber->getState() != Fiber::TERM
            && idle_fiber->getState() != Fiber::EXCEPT)
            {
                idle_fiber->m_state = Fiber::HOLD;
            }
        }

    }
}

void Scheduler::tickle()
{
    std::cout << "tickle" << std::endl;
}

bool Scheduler::stopping()
{
    MutexType::LockGuard Lokc(m_mutex);
    return m_autoStop 
    && m_stopping 
    && m_fibers.empty() 
    && m_activeThreadCount == 0;
}

void Scheduler::idle()
{
    std::cout << "idle" << std::endl;
    while(!stopping()) {
        Global::Fiber::YieldToHold();
    }
}

} // namespace Global

