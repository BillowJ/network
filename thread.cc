#include "utils.h"
#include "thread.h"
#include <iostream>

namespace Global
{

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";


Thread* Thread::GetThis()
{
    return t_thread;
}

const std::string& Thread::GetName()
{
    return t_thread_name;
}

void Thread::SetName(const std::string& name)
{
    if(name.empty()) return;
    if(t_thread)
    {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_cb(cb), m_name(name)
{
    if(m_name.empty()) m_name = "UNKNOW";
    int res = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(res)
    {
        std::cerr << "pthread_create error" << std::endl;
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();
}

Thread::~Thread()
{
    if(m_thread)
    {
        pthread_detach(m_thread);
    }
}

void Thread::join()
{
    if(m_thread)
    {
        int res = pthread_join(m_thread, nullptr);
        if(res)
        {
            std::cerr << "pthread_join error" << std::endl;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

void* Thread::run(void* arg)
{
    Thread* thread = reinterpret_cast<Thread*>(arg);
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = Global::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
    std::function<void()> cb;
    cb.swap(thread->m_cb);
    thread->m_semaphore.notify();
    cb();
    return 0;
}
} // namespace Global
