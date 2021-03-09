#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__
#include "timer.h"
#include "scheduler.h"
#include <memory>
#include <functional>
namespace Global
{
class IOManager : public Scheduler
                , public TimerManager
{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef Global::RWMutex RWMutexType;
    
    enum Event
    {
        NONE  = 0x0,
        READ  = 0x1,
        WRITE = 0x4
    };

    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name="");
    ~IOManager();

public:
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);
    bool cancelAll(int fd);
    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void onTimerInsertedAtFront() override;
    bool stopping(uint64_t& timeout);
    void contextResize(size_t size);

private:
    struct FdContext
    {
        typedef Mutex MutexType;
        struct EventContext
        {
            Scheduler* scheduler = nullptr;     // 所在调度器
            Fiber::ptr fiber;                   // 所在协程
            std::function<void()> cb;
        };
        
        EventContext& getEvent(Event event);
        void triggerEvent(Event event);
        void resetContext(EventContext& ctx);
        
        int fd = 0;
        EventContext read;                      // 读事件
        EventContext write;                     // 写事件
        Event events = NONE;                    // 事件类型
        MutexType mutex;                        
    };
    
private:

    int m_epollfd = 0;
    int m_tickleFds[2];
    std::atomic<size_t> m_pendingEventCount = {0};
    RWMutexType m_mutex;    /// IOManager的Mutex
    std::vector<FdContext*> m_fdContexts;
};



} // namespace Global


#endif