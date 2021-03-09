#include <pthread.h>
#include <stdio.h>

thread_local int item = 0;

void* func(void*)
{
    printf("child thread id: %ul, item: %d", pthread_self(), item);
    return nullptr;
}

int main()
{
    pthread_t threads[3];
    item++;
    for(int i =0 ; i < 3; i++)
    {
        pthread_create(&threads[i], nullptr, func, nullptr);
    }
    printf("thread id: %ul, item: %d\n", pthread_self(), item);
}