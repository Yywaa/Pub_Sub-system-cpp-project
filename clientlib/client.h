#include <stdio.h>
#include "../Common/comm_types.h"
#include "../Common/cmsgOp.h"

void coordinator_register(int sock_fd, const char *entity_name, msg_type_t msg_type);
void coordinator_unregister(int sock_fd, uint32_t pub_sub_id, msg_type_t msg_type);
int pub_sub_dispatch_cmsg(int sock_fd, cmsg_t *cmsg);