// Coordinator = Listener thread +{Set of Distribution Threads}

#include <pthread.h>

extern void coordinator_main();
int main(int argc, char **argv)
{
    coordinator_main();
    pthread_exit(0); // this function will make thread exit, but other threads will continue to run
    return 0;
}