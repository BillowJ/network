#include "fd_manager.h"
#include "hook.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

namespace Global
{

FdCtx::FdCtx(int fd)
    : m_fd(fd)
    , m_isInit(false)
    , m_isSocket(false)
    , m_sysNonblock(false)
    , m_userNonblock(false)
    , m_recvTimeout(-1)
    , m_sendTimeout(-1)
{
    init();
}

FdCtx::~FdCtx()
{

}

bool FdCtx::init()
{
    if(m_isInit) return true;
    
    m_recvTimeout = -1;
    m_sendTimeout = -1;

    struct stat fd_stat;
    if(-1 == fstat(m_fd, &fd_stat))
    {
        m_isInit = false;
        m_isSocket = false;
    }
    else
    {
        m_isInit = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }

    if(m_isSocket)
    {
        int flags = fcntl_f(m_fd, F_GETFL, 0);
        if(!(flags & O_NONBLOCK))
        {
            fcntl_f(m_fd, F_SETFL, flags | O_NONBLOCK);
        }
        m_sysNonblock = true;
    }
    else m_sysNonblock = false;

    m_userNonblock = false;
    m_isClose = false;
    return m_isInit;
}

void FdCtx::setTimeout(int type, uint64_t val)
{
    if(type == SO_RCVTIMEO)
    {
        m_recvTimeout = val;
    }
    else
    {
        m_sendTimeout = val;
    }
}

uint64_t FdCtx::getTimeout(int type) 
{
    if(type == SO_RCVTIMEO) 
    {
        return m_recvTimeout;
    } else 
    {
        return m_sendTimeout;
    }
}

FdManager::FdManager()
{
    m_pfds.resize(64);
}

FdCtx::ptr FdManager::get(int fd, bool auto_create)
{
    RWMutexType::ReadLock Lock(m_mutex);
    if(fd >= m_pfds.size())
    {
        if(auto_create) return nullptr;
    }
    else
    {
        if(m_pfds[fd] || !auto_create)
        {
            return m_pfds[fd];
        }
    }
    Lock.unlock();
    RWMutexType::WriteLock Lock1(m_mutex);
    FdCtx::ptr ctx(new FdCtx(fd));
    if(fd >= (int)m_pfds.size())
    {
        m_pfds.resize(fd*1.5);
    }
    m_pfds[fd] = ctx;
    return ctx;
}

void FdManager::del(int fd)
{
    RWMutexType::WriteLock Lock(m_mutex);
    if(fd >= (int)m_pfds.size()) return;
    m_pfds[fd].reset();
}

} // namespace Global
