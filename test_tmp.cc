#include "utils.h"
#include "macro.h"
#include <iostream>

void func()
{
    GLOBAL_ASSERT2(false == true, "aasdkajsd");
    std::cout << Global::BacktraceToString(10, 2);
}

int main()
{
    func();
    return 0;
}