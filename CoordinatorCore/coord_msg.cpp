#include "../Common/cmsgOp.h"
#include "../Common/comm_types.h"
#include <stdio.h>

static uint32_t coord_generate_id()
{
    static uint32_t id = 0;
    return ++id;
}

cmsg_t *coordinator_process_publisher_msg(cmsg_t *msg, size_t bytes_read)
{
    return nullptr;
    return NULL;
}
cmsg_t *coordinator_process_subscriber_msg(cmsg_t *msg, size_t bytes_read)
{
    return nullptr;
}