#ifndef __MACRO_H__
#define __MACRO_H__
#include "utils.h"

#include <iostream>
#include <assert.h>
#include <string.h>
namespace Global
{

#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#   define GLOBAL_LIKELY(x)     __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率bu成立
#   define GLOBAL_UNLIKELY(x)   __builtin_expect(!!(x), 0)
#else
#   define GLOBAL_LIKELY(X)     __buitin_expect(X)
#   define GLOBAL_UNLIKELY(X)   __builtin_expect(X)
#endif

#define GLOBAL_ASSERT(x) \
    if(!(x)) { \
        std::cout << "ASSERTINO:" #x \
        << "\nbacktrace: \n" \
        << Global::BacktraceToString(100, 2, "       "); \
        assert(x); \
    }
 

#define GLOBAL_ASSERT2(x, w) \
    if(!(x)) { \
        std::cout << "ASSERTINO:" #x \
        << "\n" << w \
        << "\nbacktrace: \n" \
        << Global::BacktraceToString(100, 2, "       "); \
        assert(x); \
    }

}

#endif