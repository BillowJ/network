#include "iomanager.h"
#include "timer.h"

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>



void test_fiber()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int flag = 1;
    // setsockopt(sockfd, SOL_SOCKET, SOCK_NONBLOCK ,&flag, sizeof(int));
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "14.215.177.38", &addr.sin_addr.s_addr);
    if(!connect(sockfd, (sockaddr*)&addr, sizeof(addr))){}
    else if(errno == EINPROGRESS)
    {
        Global::IOManager::GetThis()->addEvent(sockfd, Global::IOManager::READ, [](){
            std::cout << "read callback" << std::endl;
        });

        Global::IOManager::GetThis()->addEvent(sockfd, Global::IOManager::WRITE, [sockfd](){     // 引用捕获出错 数值变换
            std::cout << "write callback" << std::endl;
            std::cout << sockfd << std::endl;
            Global::IOManager::GetThis()->cancelEvent(sockfd, Global::IOManager::READ);
            close(sockfd);
        });
    }
    else{
        std::cout << "else" << std::endl;
    }
}

void test()
{
    Global::IOManager manager;
    manager.schedule(&test_fiber);
}

static Global::Timer::ptr s_timer;
void test_timer()
{
    Global::IOManager manager(1);
    s_timer = manager.addTimer(1000, [](){
        static int i = 0;
        std::cout << "hello timer i="  << i <<std::endl;
        if(++i == 3){
            s_timer->reset(500, true);
            // s_timer->cancel();
        }
    }, true);
}

int main(int argc, char** argv)
{
    test_timer();
}