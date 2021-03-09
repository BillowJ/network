#include "macro.h"
#include "iomanager.h"
#include "timer.h"

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

namespace Global
{

IOManager::IOManager(size_t threads, bool use_caller, const std::string& name)
    : Scheduler(threads, use_caller, name)
{
    m_epollfd = epoll_create(5000);
    GLOBAL_ASSERT(m_epollfd > 0);

    int res = pipe(m_tickleFds);
    GLOBAL_ASSERT(!res);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    res = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    GLOBAL_ASSERT(!res);

    res = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    GLOBAL_ASSERT(!res);

    contextResize(64);
    start();
}

IOManager::~IOManager()
{
    stop();
    close(m_epollfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);
    for(size_t i = 0; i < m_fdContexts.size(); i++)
    {
        if(m_fdContexts[i]) delete m_fdContexts[i];
    }

}

IOManager* IOManager::GetThis() {
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}


IOManager::FdContext::EventContext& IOManager::FdContext::getEvent(Event event)
{
    switch (event)
    {
    case IOManager::READ:
        return read;
        break;
    case IOManager::WRITE:
        return write;
        break;
    default:
        GLOBAL_ASSERT2(false, "getEvent")
    }
    throw std::invalid_argument("getContext invalid event");
}

void IOManager::FdContext::resetContext(EventContext& ctx) {
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(Event event)
{
    GLOBAL_ASSERT(events & event);
    events = (Event)(events & ~event);
    EventContext& event_ctx = getEvent(event);
    if(event_ctx.cb)
    {
        event_ctx.scheduler->schedule(&event_ctx.cb);
    }
    else if(event_ctx.fiber)
    {
        event_ctx.scheduler->schedule(&event_ctx.fiber);
    }
    event_ctx.scheduler = nullptr;
    return;
}

void IOManager::contextResize(size_t size)
{
    m_fdContexts.resize(size);
    for(size_t i = 0; i < m_fdContexts.size(); i++)
    {
        if(!m_fdContexts[i])
        {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb)
{
    FdContext* context = nullptr;
    RWMutexType::ReadLock Lock(m_mutex);
    
    // 是否越界
    if(fd < (int)m_fdContexts.size())
    {
        context = m_fdContexts[fd];
        Lock.unlock();
    }
    else if(fd >= (int)m_fdContexts.size())
    {
        Lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        contextResize(fd * 1.5);
        context = m_fdContexts[fd];
    }

    FdContext::MutexType::LockGuard lock2(context->mutex);
    if(context->events & event)         // 已经存在
    {
         std::cout << "addEvent assert fd=" << fd
                    << " event=" << (EPOLL_EVENTS)event
                    << " context.event=" << (EPOLL_EVENTS)context->events;
        GLOBAL_ASSERT(!(context->events & event));
    }
    int op = context->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epv;
    epv.events = EPOLLET | context->events | event;
    epv.data.ptr = context;
    int res = epoll_ctl(m_epollfd, op, fd, &epv);
    if(res)
    {
        std::cout << "epoll_ctl(" << m_epollfd << ", "
            << ", " << fd << ", " << (EPOLL_EVENTS)epv.events << "):"
            << res << " (" << errno << ") (" << strerror(errno) << ") context->events="
            << (EPOLL_EVENTS)context->events;
        return -1;
    }
    
    ++m_pendingEventCount;
    
    context->events = (Event)(context->events | event);
    FdContext::EventContext& event_ctx = context->getEvent(event);
    
    GLOBAL_ASSERT(!event_ctx.scheduler
                &&!event_ctx.fiber
                &&!event_ctx.cb);

    event_ctx.scheduler = Scheduler::GetThis();
    
    if(cb)
    {
        event_ctx.cb.swap(cb);
    } 
    else
    {
        event_ctx.fiber = Fiber::GetThis();
        GLOBAL_ASSERT2(event_ctx.fiber->getState() == Fiber::EXEC
                    , "state = " << event_ctx.fiber->getState());
    }
    return 0;
}

bool IOManager::delEvent(int fd, Event event)
{
    RWMutexType::ReadLock lock1(m_mutex);
    if(fd >= (int)m_fdContexts.size()) return false;
    IOManager::FdContext* fd_ctx = m_fdContexts[fd];
    lock1.unlock();

    FdContext::MutexType::LockGuard lock2(fd_ctx->mutex);
    if(!(fd_ctx->events & event))
    {
        std::cout << "event: " << event << "doesn't exist" << std::endl;
        return false;
    }

    Event new_event = (Event)(fd_ctx->events & (~event));   // 修改events
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epv;
    epv.events = new_event | EPOLLET;
    epv.data.ptr = fd_ctx;

    int res = epoll_ctl(m_epollfd, op, fd, &epv);
    if(res)
    {
        std::cout << "epoll_ctl(" << m_epollfd << ", "
            << ", " << fd << ", " << (EPOLL_EVENTS)epv.events << "):"
            << res << " (" << errno << ") (" << strerror(errno) << ") context->events="
            << (EPOLL_EVENTS)fd_ctx->events;
        return false;
    }

    --m_pendingEventCount;
    fd_ctx->events = new_event;
    FdContext::EventContext& event_ctx = fd_ctx->getEvent(event);
    fd_ctx->resetContext(event_ctx);        // 重置对应fd的event事件的上下文
    return true;
}
bool IOManager::cancelEvent(int fd, Event event)
{
    RWMutexType::ReadLock lock1(m_mutex);
    if(fd >= (int)m_fdContexts.size()) return false;
    FdContext* fd_ctx = m_fdContexts[fd];
    lock1.unlock();

    FdContext::MutexType::LockGuard lock2(fd_ctx->mutex);
    if(!(fd_ctx->events & event))
    {
        return false;
    }
    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epv;
    epv.events = new_events | EPOLLET;
    epv.data.ptr = fd_ctx;

    int res = epoll_ctl(m_epollfd, op, fd, &epv);
    if(res)
    {
        std::cout << "epoll_ctl(" << m_epollfd << ", "
            << ", " << fd << ", " << (EPOLL_EVENTS)epv.events << "):"
            << res << " (" << errno << ") (" << strerror(errno) << ") context->events="
            << (EPOLL_EVENTS)fd_ctx->events;
        return false;
    }
    fd_ctx->triggerEvent(event);
    --m_pendingEventCount;
    return true;
}

bool IOManager::cancelAll(int fd)
{
    RWMutexType::ReadLock lock(m_mutex);
    if(fd >= (int)m_fdContexts.size()) return false;
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();
    
    FdContext::MutexType::LockGuard lock2(fd_ctx->mutex);
    if(!fd_ctx->events) return false;

    int op = EPOLL_CTL_DEL;
    epoll_event epv;
    epv.events = 0;
    epv.data.ptr = fd_ctx;

    int res = epoll_ctl(m_epollfd, op, fd, &epv);
    if(res)
    {
        std::cout << "epoll_ctl(" << m_epollfd << ", "
            << ", " << fd << ", " << (EPOLL_EVENTS)epv.events << "):"
            << res << " (" << errno << ") (" << strerror(errno) << ") context->events="
            << (EPOLL_EVENTS)fd_ctx->events;
        return false;
    }
    if(fd_ctx->events & IOManager::READ)
    {
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }
    if(fd_ctx->events & IOManager::WRITE)
    {
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
    }
    GLOBAL_ASSERT(fd_ctx->events == 0);
    return true;
}

void IOManager::tickle()
{
    if(!hasIdleThreads()) return;
    std::cout << "tickle" << std::endl;
    int writed = ::write(m_tickleFds[1], "t", 1);
    GLOBAL_ASSERT(writed == 1);
}

bool IOManager::stopping()
{
    uint64_t timeout = 0;
    return stopping(timeout);
}

bool IOManager::stopping(uint64_t& timeout)
{
    timeout = getNextTimer();

    return timeout == ~0ull
        && Scheduler::stopping()
        && m_pendingEventCount == 0;
}

void IOManager::idle()
{
    std::cout << "idle" << std::endl;
    const uint64_t MAX_EVENT = 256;
    epoll_event* events = new epoll_event[MAX_EVENT]();
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
        delete[] ptr;
    });

    while (true)
    {
        uint64_t next_timeout = 0;
        if(stopping(next_timeout))
        {
            std::cout << "name = " << getName()
            << "idle stopping exit"<< std::endl;
            break;
        }
        int count = 0;
        do
        {
            static const int MAX_TIMEOUT = 3000;
            if(next_timeout != ~0ull){
                next_timeout = (int)next_timeout > MAX_TIMEOUT ? MAX_TIMEOUT : next_timeout;
            }
            else{
                next_timeout = MAX_TIMEOUT;
            }
            // std::cout << "timeout: " << next_timeout <<std::endl;
            count = epoll_wait(m_epollfd, events, MAX_EVENT, next_timeout);
            // std::cout << "epoll_wait count: " << count << std::endl;
            // std::cout << "pending event: " << m_pendingEventCount <<std::endl;
            if(count < 0 && errno == EINTR)
            { }
            else
            {
                break;
            }
        } while (true);
        
        std::vector<std::function<void()> > cbs;
        listExpiredCb(cbs);
        // std::cout << "cbs size = " << cbs.size() << std::endl;
        if(!cbs.empty())
        {
            // std::cout << "on timer cbs size=" << cbs.size();
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        for(int i = 0; i < count; i++)
        {
            epoll_event& event = events[i];
            if(event.data.fd == m_tickleFds[0])
            {
                uint8_t dummy;
                while(read(m_tickleFds[0], &dummy, sizeof(dummy)) > 0);
                continue;
            }
            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            FdContext::MutexType::LockGuard lock(fd_ctx->mutex);
            if(event.events & (EPOLLERR | EPOLLHUP))
            {
                event.events |= EPOLLIN | EPOLLOUT;
            }
            int real_event = NONE;
            if(event.events & EPOLLIN)
            {
                real_event |= READ;
            }
            if(event.events & EPOLLOUT)
            {
                real_event |= WRITE;
            }

            if((fd_ctx->events & real_event) == NONE)
            {
                continue;
            }
            int left_event = (fd_ctx->events & ~real_event);
            int op = left_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_event;

            int res = epoll_ctl(m_epollfd, op, fd_ctx->fd, &event);
            if(res)
            {
                std::cout << "epoll_ctl(" << m_epollfd << ", "
                << ", " << fd_ctx->fd << ", " << (EPOLL_EVENTS)event.events << "):"
                << res << " (" << errno << ") (" << strerror(errno) << std::endl;
                continue;
            }
            if(real_event & READ)
            {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if(real_event & WRITE)
            {
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }
        Fiber::ptr cur = Fiber::GetThis();
        auto raw_ptr = cur.get();
        cur.reset();
        raw_ptr->swapOut();
    }
    
}

void IOManager::onTimerInsertedAtFront() {
    tickle();
}

} // namespace Global
