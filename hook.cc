#include "hook.h"
#include "fiber.h"
#include "iomanager.h"
#include "fd_manager.h"
#include "macro.h"

#include <stdarg.h> 
#include <dlfcn.h>

namespace Global
{

static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(fcntl) \
    XX(ioctl) \
    XX(getsockopt) \
    XX(setsockopt)

void hook_init()
{
    static bool is_inited = false;
    if(is_inited) return;
// ##是连接符
#define XX(name) name ## _f = (name ## _fun) dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX

}

static uint64_t s_connect_timeout = -1;
struct _Hook_initer
{
    _Hook_initer(){
        hook_init();
        s_connect_timeout = 3000;
    }
};


static _Hook_initer s_hook_initer;

bool is_hook_enable()
{
    return t_hook_enable;
}

void set_hook_enable(bool flag)
{
    t_hook_enable = flag;
}

} // namespace Global

struct time_info
{
    bool canceled = false;
};


template<typename OriginFun, typename ...Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
                     uint32_t event, int timeout_so, Args&&... args)
{
    
    if(!Global::t_hook_enable)
    {
        return fun(fd, std::forward<Args>(args)...);
    }
    Global::FdCtx::ptr ctx = Global::FdMgr::GetInstance()->get(fd);
    if(!ctx)
    {
        return fun(fd, std::forward<Args>(args)...);
    }
    if(ctx->isClose())
    {
        errno = EBADF;
        return -1;
    }
    if(!ctx->isSocket() || ctx->getUserNonblock())
    {
        return fun(fd, std::forward<Args>(args)...);
    }

    uint64_t to = ctx->getTimeout(timeout_so);      // timeout_so 是类型
    std::shared_ptr<time_info> tinfo(new time_info);

    ssize_t n = -1;
    while(true)
    {
        n = fun(fd, std::forward<Args>(args)...);
        while(n == -1 && errno == EINTR)
        {
            n = fun(fd, std::forward<Args>(args)...);
        }
        if(n == -1 && errno == EAGAIN)
        {   // 超時 加定時器
            Global::IOManager* manager = Global::IOManager::GetThis();
            std::weak_ptr<time_info> info(tinfo);
            Global::Timer::ptr timer;
            if(to != (uint64_t)-1) // timeout有值
            {
                timer = manager->addConditionTimer(to, [info, fd, manager, event](){
                    auto t = info.lock();
                    if(!t || t->canceled)
                    {
                        return;
                    }
                    t->canceled = ETIMEDOUT;
                    manager->cancelEvent(fd, (Global::IOManager::Event)event);
                }, info);
            }
            int res = manager->addEvent(fd, (Global::IOManager::Event)event);
            if(res)
            {
                std::cout << hook_fun_name << "addEvent(" << fd << "," << event << ")";
                if(timer)
                {
                    timer->cancel();
                }
                return -1;
            }
            else
            {
                Global::Fiber::YieldToHold();

                if(timer)   // 正常的事件触发
                {
                    timer->cancel();
                }
                if(tinfo->canceled) // 超时
                {
                    errno = tinfo->canceled;
                    return -1;
                }
                continue;
            }
        }

        break;
    }
    return n;
    
}

// 声明
extern "C"
{
#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX);
#undef XX

unsigned int sleep(unsigned int seconds)
{
    if(!Global::t_hook_enable){
        return sleep_f(seconds);
    }
    // hook : ...
    Global::Fiber::ptr fiber = Global::Fiber::GetThis();
    Global::IOManager* manager = Global::IOManager::GetThis();
    // manager->addTimer(seconds*1000, std::bind(&Global::IOManager::schedule, manager, fiber, -1));
    manager->addTimer(seconds*1000, [manager, fiber](){
        manager->schedule(fiber);
    });
    Global::Fiber::YieldToHold();
    return 0;
}

int usleep(useconds_t usec)
{
    if(!Global::t_hook_enable){
        return usleep_f(usec);
    }
    // hook : ...
    Global::Fiber::ptr fiber = Global::Fiber::GetThis();
    Global::IOManager* manager = Global::IOManager::GetThis();
    // manager->addTimer(usec / 1000, std::bind(&Global::IOManager::schedule, manager, fiber, -1));
    manager->addTimer(usec/1000, [manager, fiber](){
        manager->schedule(fiber);       // 返回继续
    });
    Global::Fiber::YieldToHold();       // 让出cpu
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
    if(!Global::t_hook_enable)
    {
        return nanosleep(req, rem);
    }
    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 / 1000;
    Global::Fiber::ptr fiber = Global::Fiber::GetThis();
    Global::IOManager* manager = Global::IOManager::GetThis();
    manager->addTimer(timeout_ms, [manager, fiber](){
        manager->schedule(fiber);
    });
    Global::Fiber::YieldToHold();
}

int socket(int domain, int type, int protocol)
{
    if(!Global::t_hook_enable)
    {
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, protocol);
    if(fd == -1)
    {
        return fd;
    }
    Global::FdMgr::GetInstance()->get(fd, true);
    return fd;
}

int connect_with_timeout(int sockfd, const struct sockaddr *addr, socklen_t addrlen,  uint64_t timeout_ms)
{
    if(!Global::is_hook_enable)
    {
        return connect_f(sockfd, addr, addrlen);
    }
    Global::FdCtx::ptr ctx = Global::FdMgr::GetInstance()->get(sockfd);
    if(!ctx || ctx->isClose())
    {
        errno = EBADF;
        return -1;
    }

    if(!ctx->isSocket())
    {
        return connect_f(sockfd, addr, addrlen);
    }

    if(ctx->getUserNonblock()) {
        return connect_f(sockfd, addr, addrlen);
    }

    int n = connect_f(sockfd, addr, addrlen);
    if(n == 0) {
        return 0;
    } else if(n != -1 || errno != EINPROGRESS) {
        return n;
    }

    Global::IOManager* manager = Global::IOManager::GetThis();
    Global::Timer::ptr timer;
    std::shared_ptr<time_info> tinfo(new time_info);
    std::weak_ptr<time_info> winfo(tinfo);
    if(timeout_ms != (uint64_t)-1)
    {
        timer = manager->addConditionTimer(timeout_ms, [winfo, sockfd, manager]()
        {
            auto t = winfo.lock();
            if(!t || t->canceled)
            {
                return;
            }
            t->canceled = ETIMEDOUT;
            manager->cancelEvent(sockfd, Global::IOManager::WRITE);
        }, winfo);
    }
    
    int res = manager->addEvent(sockfd, Global::IOManager::WRITE);
    if(!res)
    {
        Global::Fiber::YieldToHold();
        if(timer)
        {
            timer->cancel();
        }
        if(tinfo->canceled)
        {
            errno = tinfo->canceled;
            return -1;
        }
    }
    else
    {
        if(timer)
        {
            timer->cancel();
        }
        std::cout << "connect addEvent(" << sockfd << ", WRITE) error";
    }
    int error = 0;
    socklen_t len = sizeof(int);
    if(-1 == getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &errno, &len))
    {
        return -1;
    }
    if(!errno)
    {
        return 0;
    }
    else
    {
        errno = errno;
        return -1;
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return connect_with_timeout(sockfd, addr, addrlen, Global::s_connect_timeout);
}

int accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    // TODO: 
    int fd = do_io(s, accept_f, "accept", Global::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if(fd >= 0)
    {
        Global::FdMgr::GetInstance()->get(fd, true);
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count) {
    return do_io(fd, read_f, "read", Global::IOManager::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, readv_f, "readv", Global::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", Global::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    return do_io(sockfd, recvfrom_f, "recvfrom", Global::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return do_io(sockfd, recvmsg_f, "recvmsg", Global::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return do_io(fd, write_f, "write", Global::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, writev_f, "writev", Global::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int s, const void *msg, size_t len, int flags) {
    return do_io(s, send_f, "send", Global::IOManager::WRITE, SO_SNDTIMEO, msg, len, flags);
}

ssize_t sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen) {
    return do_io(s, sendto_f, "sendto", Global::IOManager::WRITE, SO_SNDTIMEO, msg, len, flags, to, tolen);
}

ssize_t sendmsg(int s, const struct msghdr *msg, int flags) {
    return do_io(s, sendmsg_f, "sendmsg", Global::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}

int close(int fd) {
    if(!Global::t_hook_enable) {
        return close_f(fd);
    }

    Global::FdCtx::ptr ctx = Global::FdMgr::GetInstance()->get(fd);
    if(ctx) {
        auto iom = Global::IOManager::GetThis();
        if(iom) {
            iom->cancelAll(fd);
        }
        Global::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

ssize_t send(int s, const void *msg, size_t len, int flags) {
    return do_io(s, send_f, "send", Global::IOManager::WRITE, SO_SNDTIMEO, msg, len, flags);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", Global::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}


int fcntl(int fd, int cmd, ... /* arg */ )
{
    va_list va;
    va_start(va, cmd);
    switch (cmd)
    {
    case F_SETFL:
        {
            int arg = va_arg(va, int);
            va_end(va);
            Global::FdCtx::ptr ctx = Global::FdMgr::GetInstance()->get(fd);
            if(!ctx || ctx->isClose() || !ctx->isSocket())
            {
                return fcntl_f(fd, cmd, arg);
            }
            ctx->setUserNonblock(arg & O_NONBLOCK);
            if(ctx->getSysNonblock())
            {
                arg |= O_NONBLOCK;
            }
            else
            {
                arg &= ~O_NONBLOCK;
            }
            return fcntl_f(fd, cmd, arg);
        }
        
        break;
    case F_GETFL:
        {
            va_end(va);
            int arg = fcntl_f(fd, cmd);
            Global::FdCtx::ptr ctx = Global::FdMgr::GetInstance()->get(fd);
            if(!ctx || ctx->isClose() || !ctx->isSocket()) {
                return arg;
            }
            if(ctx->getUserNonblock()) {
                return arg | O_NONBLOCK;
            } else {
                return arg & ~O_NONBLOCK;
            }
        }
        break;
    case F_DUPFD:
    case F_DUPFD_CLOEXEC:
    case F_SETFD:
    case F_SETOWN:
    case F_SETSIG:
    case F_SETLEASE:
    case F_NOTIFY:
#ifdef F_SETPIPE_SZ
    case F_SETPIPE_SZ:
#endif
        {
            int arg = va_arg(va, int);
            va_end(va);
            return fcntl_f(fd, cmd, arg); 
        }
        break;
    case F_GETFD:
    case F_GETOWN:
    case F_GETSIG:
    case F_GETLEASE:
#ifdef F_GETPIPE_SZ
        case F_GETPIPE_SZ:
#endif
        {
            va_end(va);
            return fcntl_f(fd, cmd);
        }
        break;
    case F_SETLK:
    case F_SETLKW:
    case F_GETLK:
        {
            struct flock* arg = va_arg(va, struct flock*);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
        break;
    case F_GETOWN_EX:
    case F_SETOWN_EX:
        {
            struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
        break;
    default:
        va_end(va);
        return fcntl_f(fd, cmd);
    }
}

int ioctl(int d, unsigned long int request, ...)
{
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);

    if(FIONBIO == request)
    {
        bool user_nonblock= !!*(int*)arg;
        Global::FdCtx::ptr ctx =  Global::FdMgr::GetInstance()->get(d);
        if(!ctx || ctx->isClose() || !ctx->isSocket())
        {
            return ioctl_f(d, request, arg);
        }
        ctx->setUserNonblock(user_nonblock);
    }
    return ioctl_f(d, request, arg);
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if(!Global::t_hook_enable) {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if(level == SOL_SOCKET) {
        if(optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            Global::FdCtx::ptr ctx = Global::FdMgr::GetInstance()->get(sockfd);
            if(ctx) {
                const timeval* v = (const timeval*)optval;
                ctx->setTimeout(optname, v->tv_sec * 1000 + v->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}

} // extern "C"