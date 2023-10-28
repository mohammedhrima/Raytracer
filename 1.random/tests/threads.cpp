#include <iostream>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
void *threadFunction(void *arg)
{
    int id = *static_cast<int *>(arg);
    while (1)
    {
        pthread_mutex_lock(&mtx);
        std::cout << "Hello from thread " << id << std::endl;
        pthread_mutex_unlock(&mtx);
        usleep(300000);
    }
    return nullptr;
}

int main()
{
    pthread_t thread1, thread2;
    int id1 = 1, id2 = 2;

    pthread_create(&thread1, nullptr, threadFunction, &id1);
    pthread_create(&thread2, nullptr, threadFunction, &id2);

    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);

    return 0;
}
