#ifndef __CMSG_OP__
#define __CMSG_OP__

#include "comm_types.h"

void cmsg_debug_print(cmsg_t *cmsg);

cmsg_t *cmsg_data_prepare2(msg_type_t msg_type, sub_msg_type_t sub_msg_type, uint32_t msg_code, int trailing_space);

#endif