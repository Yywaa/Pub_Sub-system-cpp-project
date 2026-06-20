#ifndef __COORD_DB__
#define __COORD_DB__

#include <stdint.h>
#include "pubsub.h"
#include <unordered_map>

extern std::unordered_map<uint32_t, publisher_db_entry_t *> pub_db;

/*Publisher DB operation*/
publisher_db_entry_t *publisher_db_create(uint32_t pub_id, char *pub_name);

void publisher_db_delete(uint32_t pub_id);
bool publisher_publish_msg(uint32_t pub_id, uint32_t published_msg_id);
bool publisher_ubpublish_msg(uint32_t pub_id, uint32_t published_msg_id);

/****Subscriber DB Operations */
typedef struct subscriber_db_entry_ subscriber_db_entry_t;

std::shared_ptr<subscriber_db_entry_t> subscriber_db_create(uint32_t sub_id, char *sub_name);
void subscriber_db_delete(uint32_t sub_id);
bool subscriber_subscribe_msg(uint32_t sub_id, uint32_t msg_id);
bool subscriber_unsubscribe_msg(uint32_t sub_id, uint32_t msg_id);

/****Pub-sub DB operation */

typedef struct pub_sub_db_entry_ pub_sub_db_entry_t;
pub_sub_db_entry_t *pub_sub_db_create(uint32_t msg_id, std::shared_ptr<subscriber_db_entry_t> SubEntry);

void pub_sub_db_delete(uint32_t msg_id, uint32_t sub_id);
pub_sub_db_entry_t *pub_sub_db_get(uint32_t msg_id);
void pub_sub_db_delete_subscriber(std::shared_ptr<subscriber_db_entry_t> SubEntry);

void coord_db_display();

#endif