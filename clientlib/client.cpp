#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "../Libs/tlv.h"
#include "client.h"

int pub_sub_dispatch_cmsg(int sock_fd, cmsg_t *cmsg)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(COORD_UDP_PORT);
    server_addr.sin_addr.s_addr = htonl(COORD_IP_ADDR);
    int rc = sendto(sock_fd, (char *)cmsg, sizeof(*cmsg) + cmsg->tlv_buffer_size, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    return rc;
}
void coordinator_register(int sock_fd, const char *entity_name, msg_type_t msg_type)
{
    cmsg_t *cmsg = (cmsg_t *)calloc(1, sizeof(*cmsg) + TVL_OVERHEAD_SIZE + TLV_CODE_NAME_LEN);
    cmsg->msg_id = 0;
    cmsg->msg_type = msg_type;
    cmsg->sub_msg_type = SUB_MSG_REGISTER;
    cmsg->id.publisher_id = 0; // coordinator will generate for it
    cmsg->id.subscriber_id = 0;
    cmsg->tlv_buffer_size = TVL_OVERHEAD_SIZE + TLV_CODE_NAME_LEN;
    char *tlv_buffer = (char *)cmsg->tlv_buffer;
    tlv_buffer_inser_tlv(tlv_buffer, TLV_CODE_NAME, TLV_CODE_NAME_LEN, (char *)entity_name);
    int rc = pub_sub_dispatch_cmsg(sock_fd, cmsg); // pub_sub_dispatch_msg is used by publisher and subscriber
    if (rc < 0)
    {
        printf("Client : Error : Send Failed, errno = %d\n", errno);
    }
    cmsg_debug_print(cmsg);
    free(cmsg);
}
void coordinator_unregister(int sock_fd, uint32_t pub_sub_id, msg_type_t msg_type)
{
    cmsg_t *msg = (cmsg_t *)calloc(1, sizeof(*msg));
    msg->msg_id = 0;
    msg->msg_type = msg_type;
    msg->sub_msg_type = SUB_MSG_UNREGISTER;
    msg->id.publisher_id = pub_sub_id;
    msg->id.subscriber_id = pub_sub_id;
    msg->tlv_buffer_size = 0;
    int rc = pub_sub_dispatch_cmsg(sock_fd, msg);
    if (rc < 0)
    {
        printf("Client: Error : Send Failed, errno = %d\n", errno);
    }
    // cmsg_debug_print(msg);
    free(msg);
}