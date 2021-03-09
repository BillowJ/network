#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "fiber.h"
#include "noncopyable.h"

#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <thread>
#include <memory>
#include <string>
#include <atomic>
#include <list>

namespace Global
{

class Semaphore : Noncopyable
{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    void wait();
    void notify();

private:
    sem_t m_semaphore;
};

template<class T>
class ScopedLockImpl
{
public:
    ScopedLockImpl(T& mtx)
        : m_mutex(mtx)
    {
        m_mutex.lock();
        m_locked = true;
    }
    
    ~ScopedLockImpl()
    {
        m_mutex.unlock();
    }

    void lock()
    {
        if(!m_locked)
        {
            m_mutex.lock();
            m_locked = false;
        }
    }

    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
class ReadScopedLockImpl
{
public:
    ReadScopedLockImpl(T& mtx)
        : m_mutex(mtx)
    {
        m_mutex.rdlock();
        m_locked = true;
    }
    
    ~ReadScopedLockImpl()
    {
        m_mutex.unlock();
    }

    void lock()
    {
        if(!m_locked)
        {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
class WriteScopedLockImpl
{
public:
    WriteScopedLockImpl(T& mtx)
        : m_mutex(mtx)
    {
        m_mutex.wrlock();
        m_locked = true;
    }
    
    ~WriteScopedLockImpl()
    {
        m_mutex.unlock();
    }

    void lock()
    {
        if(!m_locked)
        {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

class Mutex : Noncopyable
{
public:
    typedef ScopedLockImpl<Mutex> LockGuard;
    Mutex() { pthread_mutex_init(&m_mutex, nullptr); }
    ~Mutex() { pthread_mutex_destroy(&m_mutex); }

    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

class RWMutex : Noncopyable
{
public: 
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;

    RWMutex() { pthread_rwlock_init(&m_mutex, nullptr); }
    ~RWMutex() { pthread_rwlock_destroy(&m_mutex); }

    void rdlock()
    {
        pthread_rwlock_rdlock(&m_mutex);
    }

    void wrlock()
    {
        pthread_rwlock_wrlock(&m_mutex);
    }

    void unlock()
    {
        pthread_rwlock_unlock(&m_mutex);
    }
private:
    pthread_rwlock_t m_mutex;
};

class Spinlock : Noncopyable
{
public:
    typedef ScopedLockImpl<Spinlock> LockGuard;
    Spinlock() { pthread_spin_init(&m_mutex, 0); }
    ~Spinlock() { pthread_spin_destroy(&m_mutex); }
    void lock() { pthread_spin_lock(&m_mutex); }
    void unlock() { pthread_spin_unlock(&m_mutex); }
private:
    pthread_spinlock_t m_mutex;
};

class CASlock
{
public:
    typedef ScopedLockImpl<CASlock> LockGuard;

    CASlock() { m_mutex.clear(); }

    ~CASlock() {
    } 

    void lock() {
        while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock() {
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }
    
private:
    volatile std::atomic_flag m_mutex;
};


} // namespace Global


#endif