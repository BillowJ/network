#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__
#include "fiber.h"
#include "thread.h"

#include <memory>
#include <vector>
#include <functional>

namespace Global
{


class Mutex;

class Scheduler
{
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;
    // use_caller 是否将初始化协程调度器的线程加入调度范围里
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();
    const std::string& getName() const { return m_name; }

    void start();
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb f, int thread = -1)
    {
        bool need_tickle = false;
        {
            MutexType::LockGuard Lock(m_mutex);
            need_tickle = scheduleNoLock(f, thread);
        }
        if(need_tickle)
        {
            tickle();
        }
    }

    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end)
    {
        bool need_tickle = false;
        {
            MutexType::LockGuard Lock(m_mutex);
            while(begin != end)     // 用于批量处理超时定时器
            {
                need_tickle = scheduleNoLock(&*begin, -1) || need_tickle;
                ++begin;
            }
        }
        if(need_tickle) tickle();
    }
     

    static Scheduler* GetThis();
    static Fiber* GetMainFiber();

protected:
    virtual void tickle();
    virtual bool stopping();
    virtual void idle();
    
    void run();
    void setThis();
    bool hasIdleThreads() { return m_idleThreadCount > 0; }
    
private:
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb f, int thread)
    {
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(f, thread);
        if(ft.fiber || ft.cb)
        {
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }
private:
    struct FiberAndThread
    {

        FiberAndThread(Fiber::ptr f, int thr)
            : fiber(f), thread(thr) { }

        FiberAndThread(Fiber::ptr* f, int thr)
            : thread(thr)
        {
            this->fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> c, int thr)
            : thread(thr), cb(c) { }

        FiberAndThread(std::function<void()>* c, int thr)
            : thread(thr) 
        { 
            this->cb.swap(*c);
        }
        FiberAndThread()
            : thread(-1) { }

        void reset() 
        {
            thread = -1;
            fiber = nullptr;
            cb = nullptr;
        }

        std::function<void()> cb;
        Fiber::ptr fiber; 
        int thread;
    };

protected:
    std::vector<int> m_threadIds;
    size_t m_threadCount;
    std::atomic<size_t> m_activeThreadCount = {0};
    std::atomic<size_t> m_idleThreadCount = {0};
    bool m_stopping = true;
    bool m_autoStop = false;
    int  m_rootThread = 0;

private:
    MutexType m_mutex;
    Fiber::ptr m_rootFiber;
    std::vector<Thread::ptr> m_threads;     // thread pool
    std::list<FiberAndThread> m_fibers;
    std::string m_name;
};


} // namespace Global
#endif