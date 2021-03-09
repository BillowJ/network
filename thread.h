#ifndef __THREAD_H__
#define __THREAD_H__

#include "mutex.h"

#include <string>

namespace Global
{

class Thread
{
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();
    pid_t getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    void join();

public:
    static Thread* GetThis();
    static const std::string& GetName(); 
    static void SetName(const std::string& name);

private:
    static void* run(void* arg);    // ?

private:
    pid_t m_id = -1;
    std::string m_name;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
    Semaphore m_semaphore;
};

} // namespace Global


#endif