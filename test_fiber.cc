#include "scheduler.h"
#include "fiber.h"
#include <iostream>

void run_in_fiber() {
    std::cout << "run_in_fiber begin" << std::endl;
    Global::Fiber::YieldToHold();
    std::cout << "run_in_fiber end" << std::endl;
    Global::Fiber::YieldToHold();   // 这里需要mainFiber再次swapIn方可结束
    // ...这里还未结束
}

void thread_test_fiber()
{
    Global::Scheduler sc;
    {
        
        Global::Fiber::GetThis();
        std::cout << "main begin" << std::endl;
        Global::Fiber::ptr fiber(new Global::Fiber(run_in_fiber));
        fiber->swapIn();
        std::cout << "main after swapIn" << std::endl;
        fiber->swapIn();
        std::cout << "main after end" << std::endl;
        fiber->swapIn();
    }
    
    std::cout << "main end" << std::endl;
}

int main(int argc, char** argv)
{
    {
        Global::Fiber::GetThis();
        std::cout << "main begin" << std::endl;
        Global::Fiber::ptr fiber(new Global::Fiber(run_in_fiber));
        fiber->swapIn();
        std::cout << "main after swapIn" << std::endl;
        fiber->swapIn();
        std::cout << "main after end" << std::endl;
        fiber->swapIn();
    }
    
    std::cout << "main end" << std::endl;

}