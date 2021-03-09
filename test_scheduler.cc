#include "utils.h"
#include "scheduler.h"
#include <iostream>
#include <unistd.h>

namespace Global{

void test_fiber()
{
    std::cout << "test in fiber" << std::endl;
    // sleep(1);
}
void test_fiber1() {
    static int s_count = 5;
    std::cout << "test in fiber s_count=" << s_count << std::endl;

    sleep(1);
    if(--s_count >= 0) {
        Global::Scheduler::GetThis()->schedule(&test_fiber1);
        // Global::Scheduler::GetThis()->schedule(&test_fiber1, Global::GetThreadId());
    }
}

}


int main()
{
    std::cout << "main" << std::endl;
    Global::Scheduler sc(3, true, "test");
    sc.start();
    std::cout << "schedule" << std::endl;
    sc.schedule(&Global::test_fiber1);
    sc.stop();
    std::cout << "over" << std::endl;
    return 0;
}