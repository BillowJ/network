#include "scheduler.h"
#include "fiber.h"
#include "macro.h"
#include <atomic>

namespace Global
{
static std::atomic<uint64_t> s_fiber_id{0};
static std::atomic<uint64_t> s_fiber_count{0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

Fiber::Fiber()      // 主协程无任何参数
{
    m_state = EXEC;
    SetThis(this);
    if(getcontext(&m_ctx))
    {
        GLOBAL_ASSERT2(false, "getcontent");
    }
    ++s_fiber_count;
    std::cout << "Fiber id = " << m_id << std::endl;
}

Fiber::Fiber(std::function<void()> cb, size_t stack_size, bool use_caller)       // 真正运行协程
    : m_id(++s_fiber_id)
    , m_cb(cb)
{
    std::cout << "Fiber() id = " << m_id << std::endl;
    ++s_fiber_count;
    m_stacksize = stack_size ? stack_size : 1024 * 1024;
    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx))
    {
        perror("Fiber construct");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    if(!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }
}

Fiber::~Fiber()
{
    --s_fiber_count;
    if(m_stack)
    {
        assert(m_state == TERM || m_state == INIT || m_state == EXCEPT);
        StackAllocator::Dealloc(m_stack, m_stacksize);
    }
    else    // 主协程
    {
        assert(!m_cb);
        assert(m_state == EXEC);
        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);   // 销毁主协程
        }
    }
    std::cout << "~Fiber() id = " << m_id << std::endl;
}

void Fiber::reset(std::function<void()> cb)
{
    assert(m_stack);
    assert(m_state == TERM
        || m_state == INIT
        || m_state == EXCEPT);

    m_cb = cb;
    if(getcontext(&m_ctx))
    {
        perror("Fiber construct");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void Fiber::back()
{
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        perror("back");
    }
}

void Fiber::call()
{
    SetThis(this);
    assert(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)){
        perror("call");
    }
}

void Fiber::swapIn()
{
    SetThis(this);
    assert(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext(&Global::Scheduler::GetMainFiber()->m_ctx, &m_ctx)){
        perror("swapIn");
    }
}

void Fiber::swapOut()
{
    SetThis(Scheduler::GetMainFiber());
    if(swapcontext(&m_ctx, &Global::Scheduler::GetMainFiber()->m_ctx)){
        perror("swapOut");
    }
}

void Fiber::SetThis(Fiber* f)
{
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis()
{
    if(t_fiber) return t_fiber->shared_from_this();

    Fiber::ptr main_fiber(new Fiber);
    assert(main_fiber.get() == t_fiber);
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

void Fiber::YieldToReady()
{
    Fiber::ptr cur = GetThis();
    assert(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold()
{
    Fiber::ptr cur = GetThis();
    assert(cur->m_state == EXEC);
    // cur->m_state = HOLD;
    cur->swapOut();
}

void Fiber::MainFunc()
{
    Fiber::ptr cur = GetThis();
    assert(cur);
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }
    catch(const std::exception& e)
    {
        cur->m_state = EXCEPT;
        std::cerr << e.what() << '\n';
    }
    catch(...)
    {
        cur->m_state = EXCEPT;
        std::cerr << "UNKNWO ERROR" << '\n';
    }
    // 这里为了释放协程 使用raw pointer进行swapOut
    auto raw = cur.get();
    cur.reset();
    raw->swapOut();
    // 下面部分将不会执行到,同时swapOut之后我们的协程智能指针计数到0,直接析构协程及其他持有的栈内存等.
    printf("never reache\n");
    assert(false);
}

void Fiber::CallerMainFunc()
{
    Fiber::ptr cur = GetThis();
    assert(cur);
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }
    catch(const std::exception& e)
    {
        cur->m_state = EXCEPT;
        std::cerr << e.what() << '\n';
    }
    catch(...)
    {
        cur->m_state = EXCEPT;
        std::cerr << "UNKNWO ERROR" << '\n';
    }
    // 这里为了释放协程 使用raw pointer进行swapOut
    auto raw = cur.get();
    cur.reset();
    raw->back();
    // 下面部分将不会执行到,同时swapOut之后我们的协程智能指针计数到0,直接析构协程及其他持有的栈内存等.
    printf("never reache\n");
    assert(false);
}

}