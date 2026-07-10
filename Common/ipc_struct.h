#ifndef __COORD_IPC_H__
#define __COORD_IPC_H__

#include <stdint.h>
#include "comm_types.h"
typedef struct cmsg_ cmsg_t;
typedef void (*pub_sub_cbk_t)(cmsg_t);

typedef union ipc_struct
{
    // network socket
    struct
    {
        uint32_t ip_addr;
        uint16_t port;
        uint8_t transport_type; // now it is only for UDP
        /*Dynamically computed*/
        int sock_fd;
    } netskt;
    // Msg Qs, a string
    struct
    {
        char MsgQName[64]; // a string
    } msgq;
    // Unix socket
    struct
    {
        uint32_t UnixSktName[64]; // unique name on machine level
    } uxskt;

    // function pointer; callback method
    struct
    {
        pub_sub_cbk_t cbk;
    } cbk;
} ipc_struct_t;

#endif