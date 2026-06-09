#include "pubsub.h"
#include <unordered_map>
#include "coorDB.h"
#include <stdexcept>

std::unordered_map<uint32_t, publisher_db_entry_t *> pub_db;                  // a collection of publishers
std::unordered_map<uint32_t, std ::shared_ptr<subscriber_db_entry_t>> sub_db; // a collection of subscribers, first para  is message ID subscriber subscribed to
std::unordered_map<uint32_t, pub_sub_db_entry_t *> pub_sub_db;                // first para is message ID, second is entry of subscribers subscribed to this message

/**********Below is for publisher**********/
publisher_db_entry_t *publisher_db_create(uint32_t pub_id, char *pub_name)
{
    publisher_db_entry_t *pub_entry;
    /*check if the publisher already exits*/
    if (pub_db.find(pub_id) != pub_db.end())
    {
        throw std::runtime_error("Publisher already exits");
    }
    pub_entry = new publisher_db_entry_t;
    if (pub_entry == NULL)
    {
        throw std::runtime_error("Memory allocation failed for publisher entry");
    }
    pub_entry->publisher_id = pub_id;
    strncpy(pub_entry->pub_name, pub_name, 64);
    pub_db[pub_id] = pub_entry;
    return pub_entry;
}
void publisher_db_delete(uint32_t pub_id)
{
    if (pub_db.find(pub_id) == pub_db.end())
    {
        throw std::runtime_error("Publisher does not exits!");
        return;
    }
    delete pub_db[pub_id]; // pub_id = NULL ? memry leakage?
    pub_db.erase(pub_id);

    /**below my solution */

    /*
    auto iter = pub_db.find(pub_id);
    if (iter != pub_db.end())
    {
        pub_db.erase(iter);
    }*/
}

bool publisher_publish_msg(uint32_t pub_id, uint32_t published_msg_id)
{
    publisher_db_entry_t *PubEntry = NULL;
    auto it = pub_db.find(pub_id);
    if (it == pub_db.end())
    {
        return false;
    }
    PubEntry = it->second;
    // int i;
    for (int i = 0; i < MAX_PUBLISHED_MSG; i++)
    {
        if (PubEntry->published_msg_ids[i] == published_msg_id)
            return false;
    }
    for (int i = 0; i < MAX_PUBLISHED_MSG; i++)
    {
        if (PubEntry->published_msg_ids[i]) // if message id is not zero, means publised?
        {
            continue;
        }
        PubEntry->published_msg_ids[i] = published_msg_id;
        printf("Coordinator: Publisher %s published message %uSuccessfully\n", PubEntry->pub_name, published_msg_id);
        return true;
    }
    return false;
}
bool publisher_unpublish_msg(uint32_t pub_id, uint32_t published_msg_id)
{
    publisher_db_entry_t *PubEntry = NULL;
    auto it = pub_db.find(pub_id);
    if (it == pub_db.end())
    {
        return false;
    }
    PubEntry = it->second;
    for (int i = 0; i < MAX_PUBLISHED_MSG; i++)
    {
        if (PubEntry->published_msg_ids[i] == published_msg_id)
        {
            PubEntry->published_msg_ids[i] = 0;
            return true;
        }
    }
    return false;
}

/*******Below is for subscriber********/
std::shared_ptr<subscriber_db_entry_t> subscriber_db_create(uint32_t sub_id, char *sub_name)
{
    /*check if subscriber exist or not*/
    if (sub_db.find(sub_id) != sub_db.end())
    {
        printf("Subscriber with ID %u already exits", sub_id);
    }
    /*create a new subscriber and insert it into database*/
    auto new_subscriber = std::make_shared<subscriber_db_entry_t>();
    std::shared_ptr<subscriber_db_entry_t> new_subscriber = std::make_shared<subscriber_db_entry_t>();
    // std::shared_ptr<subscriber_db_entry_t> new_subscriber = std::make_shared<subscriber_db_entry_t>(sub_id, sub_name);//default constructor is not enough for this!
    new_subscriber->subsriber_id = sub_id;
    strncpy(new_subscriber->sub_name, sub_name, sizeof(new_subscriber->sub_name));
    sub_db[sub_id] = new_subscriber;
    return new_subscriber;
}
