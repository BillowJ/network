#include "utils.h"

#include <sys/time.h>
#include <sstream>
#include <execinfo.h>

namespace Global
{

pid_t GetThreadId()
{
    return syscall(SYS_gettid);
}

uint64_t GetCurrentMs()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000ul + t.tv_usec / 1000;
}

// size = 层数 skip = 起点
void Backtrace(std::vector<std::string>& bt, int size, int skip)
{
    void** array = (void**)malloc((sizeof(void*) * size));
    size_t s = ::backtrace(array, size);

    char** strings = ::backtrace_symbols(array, s);
    if(strings == NULL)
    {
        /// ERROR
        perror("::backtrace\n");
        return;
    }
    for(size_t i = skip; i < s; i++)
    {
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix)
{
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); i++)
    {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

}