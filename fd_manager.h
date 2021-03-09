#ifndef __FD_MANAGER_H__
#define __FD_MANAGER_H__

#include "thread.h"
#include "singleton.h"

#include <memory>
#include <vector>

namespace Global
{

class FdCtx : public std::enable_shared_from_this<FdCtx>
{
public:
    typedef std::shared_ptr<FdCtx> ptr;
    FdCtx(int fd);
    ~FdCtx();
    
    bool isInit() const { return m_isInit; }
    bool isSocket() const { return m_isSocket; }
    bool isClose() const { return m_isClose; }
    void setUserNonblock(bool val) { m_userNonblock = val; }
    bool getUserNonblock() const { return m_userNonblock; }
    bool getSysNonblock() const { return m_sysNonblock; }
    void setSysrNonblock(bool val) { m_sysNonblock = val; }
    void setTimeout(int type, uint64_t v);
    uint64_t getTimeout(int type);

private:
    bool init();
private:
    int  m_fd;  
    bool m_isInit: 1;
    bool m_isSocket: 1;
    bool m_sysNonblock: 1;
    bool m_userNonblock: 1;
    bool m_isClose: 1;
    /// 读超时时间毫秒
    uint64_t m_recvTimeout;
    /// 写超时时间毫秒
    uint64_t m_sendTimeout;

};  


class FdManager 
{
public:
    typedef RWMutex RWMutexType;
    FdManager();
    FdCtx::ptr get(int fd, bool auto_create = false);
    void del(int fd);

private:
    RWMutexType m_mutex;
    std::vector<FdCtx::ptr> m_pfds;
};

typedef Singleton<FdManager> FdMgr;

} // namespace Global



#endif