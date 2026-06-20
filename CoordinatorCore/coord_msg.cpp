#include "../Common/cmsgOp.h"
#include "../Common/comm_types.h"
#include <stdio.h>
#include <cassert>
#include "../Libs/tlv.h"
#include <stdio.h>
#include "pubsub.h"
#include "coordDB.h"

// extern std::unordered_map<uint32_t, publisher_db_entry_t *> pub_db;

static uint32_t coord_generate_id()
{
    static uint32_t id = 0;
    return ++id;
}

cmsg_t *coordinator_process_publisher_msg(cmsg_t *msg, size_t bytes_read)
{
    assert(msg->msg_type == PUB_TO_COORD);
    msg->msg_id = coord_generate_id();
    switch (msg->sub_msg_type)
    {
    case SUB_MSG_REGISTER:
    {
        // extract the name TLV from this Register msg
        char *tlv_buffer = (char *)msg->tlv_buffer;
        size_t tlv_buffer_size = msg->tlv_buffer_size;
        uint8_t tlv_data_len = 0;

        char *pub_name = tlv_buffer_get_particualr_tlv(tlv_buffer, tlv_buffer_size, TLV_CODE_NAME, &tlv_data_len);
        if (!pub_name)
        {
            printf("Coordinator: Error: Publisher Registeration :Publisher name TLV Missing\n");
            return cmsg_data_prepare2(COORD_TO_PUB, SUB_MSG_ERROR, ERROR_TLV_MISSING, 0); // send message back to publisher,
        }
        publisher_db_entry_t *PubEntry = publisher_db_create(coord_generate_id(), pub_name);
        cmsg_t *reply_msg = cmsg_data_prepare2(COORD_TO_PUB, SUB_MSG_ID_ALLOC_SUCCESS, 0, 0);
        reply_msg->id.publisher_id = PubEntry->publisher_id;
        printf("Coordiantor: New Publisher Registered with Pub ID %u\n", PubEntry->publisher_id);
        return reply_msg; // send success registration back to publisher
    }
    break;
    case SUB_MSG_UNREGISTER:
    {
        auto it = pub_db.find(msg->id.publisher_id);
        if (it != pub_db.end())
        {
            publisher_db_delete(msg->id.publisher_id);
            printf("Coordinator: Publisher id: %u Un-Registered\n", msg->id.publisher_id);
            // cmsg_t *reply_msg = cmsg_data_prepare2(COORD_TO_PUB, SUB_MSG_UNREGISTER, 0, 0);
            // return reply_msg;
        }
    }
    break;

    default:
        break;
    }
    return nullptr;
}
cmsg_t *coordinator_process_subscriber_msg(cmsg_t *msg, size_t bytes_read)
{
    return nullptr;
}