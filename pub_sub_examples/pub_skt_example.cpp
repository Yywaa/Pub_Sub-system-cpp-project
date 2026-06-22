#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include "../Common/comm_types.h"
#include "../clientlib/client.h"

// udp port no
#define PUB_SKT_UDP_PORT_NO 50000
uint16_t argument_port_no = 0;

void *pub_skt_example(void *arg)
{
    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd == -1)
    {
        printf("Error : Socket Creation Failed\n");
        return 0;
    }
    struct sockaddr_in self_addr;
    self_addr.sin_family = AF_INET;
    self_addr.sin_port = htons(PUB_SKT_UDP_PORT_NO);
    self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock_fd, (struct sockaddr *)&self_addr, sizeof(struct sockaddr)))
    {
        printf("Coordiantor : Error :bind failed\n");
        close(sock_fd);
        exit(1);
    }
    int rc;
    cmsg_t cmsg;
    coordinator_register(sock_fd, "Pub1", PUB_TO_COORD);
    rc = recvfrom(sock_fd, (char *)&cmsg, sizeof(cmsg), 0, NULL, NULL); // wait response from coordinator
    printf("Pub Msg ID allocated = %u\n", cmsg.id.publisher_id);
    int pub_id = cmsg.id.publisher_id;

    printf("Press any key to publish message 100\n");

    getchar();
    publisher_publish(sock_fd, pub_id, 100);

    printf("Press any key to publish message 200\n");

    getchar();
    publisher_publish(sock_fd, pub_id, 200);

    printf("Press any key to unpublish message 100\n");
    getchar();
    publisher_unpublish(sock_fd, pub_id, 100);

    printf("Press any key to unpublish message 200\n");
    publisher_unpublish(sock_fd, pub_id, 200);

    printf("Press any key to Unregister Publiser\n");
    getchar();
    coordinator_unregister(sock_fd, pub_id, PUB_TO_COORD); // publisher unregister message, will receive nothing from coordinator!
    close(sock_fd);
    return NULL;
}