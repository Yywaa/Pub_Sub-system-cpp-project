#include <stdio.h>

static void coordinator_fork_listener_thread();


/*
 *this funciton is used to create 2 threads, one is used to listen to UDP port,
 *antoher is used for distribution thread
 *
 */
void coordinator_main()
{
    coordinator_fork_listener_thread();
    // coordiantor_for_distributon_threads();
    while (true)
    {
        coord_db_display();
        getchar(); //
    }
}