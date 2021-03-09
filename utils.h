#ifndef __UTILS_H__
#define __UTILS_H__

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/syscall.h>

namespace Global
{
pid_t GetThreadId();

void Backtrace(std::vector<std::string>& bt, int size, int skip);

std::string BacktraceToString(int size, int skip, const std::string& prefix="");

uint64_t GetCurrentMs();
} // namespace sylar


#endif