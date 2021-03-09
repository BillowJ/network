#include "timer.h"
#include "utils.h"
#include <iostream>

namespace Global
{
    
bool Timer::Comparator::operator()(const Timer::ptr& left, const Timer::ptr& right) const
{
    if(!left && !right)
        return false;
    if(!left)
        return true;
    if(!right)
        return false;
    if(left->m_next < right->m_next)
        return true;
    if(left->m_next >= right->m_next)
        return false;
    return left.get() < right.get();
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager)
    : m_recurring(recurring)
    , m_ms(ms)
    , m_cb(cb)
    , m_manager(manager)
{
    m_next = Global::GetCurrentMs() + m_ms;
}

Timer::Timer(uint64_t next)
    :m_next(next) {
}

bool Timer::cancel()
{
    TimerManager::TMutexType::LockGuard Lock(m_manager->m_Tmutex);
    // TimerManager::TRWMutexType::WriteLock Lock(m_manager->m_Tmutex);
    if(m_cb)
    {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}

bool Timer::refresh()
{
    TimerManager::TMutexType::LockGuard lock(m_manager->m_Tmutex);
    // TimerManager::TRWMutexType::WriteLock lock(m_manager->m_Tmutex);
    if(!m_cb) return false;

    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()) return false;
    m_manager->m_timers.erase(it);
    m_next = Global::GetCurrentMs() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}

bool Timer::reset(uint64_t ms, bool from_now)
{
    if(ms == m_ms && !from_now) {
        return true;
    }
    TimerManager::TMutexType::LockGuard lock(m_manager->m_Tmutex);
    // TimerManager::TRWMutexType::WriteLock lock(m_manager->m_Tmutex);
    if(!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    uint64_t start = 0;
    if(from_now) {
        start = Global::GetCurrentMs();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    m_manager->addTimer(shared_from_this(), lock);
    return true;
}

TimerManager::TimerManager()
{
    m_previousTime = Global::GetCurrentMs();
}

TimerManager::~TimerManager()
{

}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring)
{
    Timer::ptr t(new Timer(ms, cb, recurring, this));
    TMutexType::LockGuard Lock(m_Tmutex);
    // TRWMutexType::WriteLock Lock(m_Tmutex);
    addTimer(t, Lock);
    std::cout << "add timer\n";
    return t;
}

void TimerManager::addTimer(Timer::ptr val, TMutexType::LockGuard& Lock)
// void TimerManager::addTimer(Timer::ptr val, TRWMutexType::WriteLock& Lock)

{
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin()) && !m_tickled;
    if(at_front) m_tickled = true;
    Lock.unlock();
    if(at_front)
    {
        onTimerInsertedAtFront();
    }
        
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb)
{
    std::shared_ptr<void> tmp = weak_cond.lock();
    if(tmp)
    {
        cb();
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb,
                                    std::weak_ptr<void> weak_cond, bool recurring)
{
    return addTimer(ms, std::bind(&OnTimer ,weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer()
{
    TMutexType::LockGuard Lock(m_Tmutex);
    // TRWMutexType::ReadLock Lock(m_Tmutex);
    m_tickled = false;
    if(m_timers.empty())
    {
        return ~0ull;       // unsigned long long
    }
    const Timer::ptr& next = *m_timers.begin();
    uint64_t now_ms = Global::GetCurrentMs();
    if(now_ms >= next->m_next)
    {
        return 0;
    }
    else{
        return next->m_next - now_ms;
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs)
{
    uint64_t now = Global::GetCurrentMs();
    std::vector<Timer::ptr> expireds;

    {
        TMutexType::LockGuard Lock(m_Tmutex);
        // TRWMutexType::WriteLock Lock(m_Tmutex);
        if(m_timers.empty()) return;
    }

    // std::cout << "m_timers: " <<m_timers.size() << std::endl;

    TMutexType::LockGuard Lock(m_Tmutex);
    // TRWMutexType::WriteLock Lock(m_Tmutex);
    if(m_timers.empty()) return;
    bool rollover = detectClockRollover(now);
    if(!rollover && (*m_timers.begin())->m_next > now)
    {
        return;
    }
    Timer::ptr now_timer(new Timer(now));
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while (it != m_timers.end() && (*it)->m_next == now)
    {
        ++it;
    }
    expireds.insert(expireds.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expireds.size());
    
    for(auto& timer : expireds)
    {
        cbs.push_back(timer->m_cb);
        if(timer->m_recurring)
        {
            timer->m_next = now + timer->m_ms;
            m_timers.insert(timer);
        }
        else
        {
            timer->m_cb = nullptr;
        }
    }
    
}

bool TimerManager::detectClockRollover(uint64_t now_ms)
{
    bool rollover = false;
    if(now_ms < m_previousTime &&
        now_ms < (m_previousTime - 60 * 60 * 1000))
    {
        rollover = true;
    }
    m_previousTime = now_ms;
    return rollover;
}

bool TimerManager::hasTimer()
{
    // 多线程访问 
    TMutexType::LockGuard Lock(m_Tmutex);
    // TRWMutexType::ReadLock Lock(m_Tmutex);
    return !m_timers.empty();
}


} // namespace Global
