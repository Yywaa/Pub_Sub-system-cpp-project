#include "pubsub.h"
#include <unordered_map>
#include "coordDB.h"
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <assert.h>
#include <algorithm>
#include <arpa/inet.h>
#include <stddef.h>
#include "pubsub.h"

std::unordered_map<uint32_t, publisher_db_entry_t *> pub_db;                  // a collection of publishers,first para is message ID it published, second is publisher instance
std::unordered_map<uint32_t, std ::shared_ptr<subscriber_db_entry_t>> sub_db; // a collection of subscribers, first para  is message ID subscriber subscribed to
std::unordered_map<uint32_t, pub_sub_db_entry_t *> pub_sub_db;                // first para is message ID, second is list of subscribers subscribed to this message

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

void subscriber_db_delete(uint32_t sub_id)
{
    if (sub_db.find(sub_id) != sub_db.end())
    {
        pub_sub_db_delete_subscriber(sub_db[sub_id]);
        sub_db.erase(sub_id);
        printf("Subsciber with ID %u deleted.\n", sub_id);
    }
    else
    {
        printf("Subscriber with ID %u Not Found\n", sub_id);
    }
}

bool subscriber_subscribe_msg(uint32_t sub_id, uint32_t msg_id)
{
    subscriber_db_entry_t *SubEntry;
    auto it = sub_db.find(sub_id);
    if (it == sub_db.end())
    {
        printf("%s():Error:Subscriber with ID%u not Found!\n", __FUNCTION__, sub_id);
        return false;
    }
    // Add the message ID to the subscriber's subscribed messages
    SubEntry = it->second.get();
    for (int i = 0; i < MAX_SUBSCRIBED_MSG; i++)
    {
        if (SubEntry->subscriber_msg_ids[i])
        {
            continue;
        }
        else
        {
            SubEntry->subscriber_msg_ids[i] = msg_id;
            printf("Coordinator: Subscriber %s subscribed to message%u Successfully\n", SubEntry->sub_name, msg_id);
            break;
        }
    }
    /*Update published subscriber database*/
    pub_sub_db_create(msg_id, it->second);
    return true;
}

bool subscriber_unsubscribe_msg(uint32_t sub_id, uint32_t msg_id)
{
    auto it = sub_db.find(sub_id);
    if (it == sub_db.end())
    {
        printf("%s(): Error: Subscriber with ID %u not Found,\n", __FUNCTION__, sub_id);
        return false;
    }
    auto SubEntry = it->second;
    for (int i = 0; i < MAX_SUBSCRIBED_MSG; i++)
    {
        if (SubEntry->subscriber_msg_ids[i] == msg_id)
        {
            SubEntry->subscriber_msg_ids[i] = 0;
            break;
        }
    }
    /*Update publisher subscriber database*/
    pub_sub_db_delete(msg_id, sub_id);
    return true;
}

/*Below is for update pub_sub_db_entry_t, a message ID, and list of subscribers subscribed to it*/
pub_sub_db_entry_t *pub_sub_db_create(uint32_t msg_id, std::shared_ptr<subscriber_db_entry_t> SubEntry)
{
    auto it = pub_sub_db.find(msg_id);
    if (it == pub_sub_db.end())
    {
        pub_sub_db_entry_t *new_entry = new pub_sub_db_entry_t();
        new_entry->publish_msg_code = msg_id;
        new_entry->subscribers.push_back(SubEntry);
        pub_sub_db[msg_id] = new_entry;
        std::cout << "Created a new pub_sub_db entry fro msg_id: " << msg_id << "\n"
                  << std::endl;
        return new_entry;
    }
    // Update existing entry
    pub_sub_db_entry_t *existing_entry = it->second;
    it->second->subscribers.push_back(SubEntry);
    existing_entry->subscribers.push_back(SubEntry);
    std::cout << "Updated exiting pub_sub_db entry for msg_id" << msg_id << std::endl;
    return existing_entry;
}
void pub_sub_db_delete(uint32_t msg_id, uint32_t sub_id)
{
    auto it = pub_sub_db.find(msg_id);
    if (it == pub_sub_db.end())
    {
        std::cout << "No pub_sub_db entry found for msg_id" << msg_id << "\n"
                  << std::endl;
        return;
    }
    pub_sub_db_entry_t *entry = it->second;
    // Find and remove the subscriber
    auto &subscribers = entry->subscribers;
    for (auto iter = subscribers.begin(); iter != subscribers.end(); iter++)
    {
        if ((*iter)->subsriber_id == sub_id)
        {
            subscribers.erase(iter);
        }
        std::cout << "Subscriber with ID " << sub_id << "removed from msg_id" << msg_id << std::endl;
        break;
    }
    // if no more subscribes, delete the entry
    if (subscribers.empty())
    {
        delete entry;
        pub_sub_db.erase(it);
        std::cout << "Deleted pub_sub_db entyr for msg_id" << msg_id << std::endl;
    }
}

pub_sub_db_entry_t *pub_sub_db_get(uint32_t msg_id)
{
    auto it = pub_sub_db.find(msg_id);

    if (it == pub_sub_db.end())
    {
        std::cout << "No pub_sub_db entry found for msg_id" << msg_id << "\n"
                  << std::endl;
        return nullptr;
    }
    return it->second;
}

void pub_sub_db_delete_subscriber(std::shared_ptr<subscriber_db_entry_t> SubEntry)
{
    for (auto it = pub_sub_db.begin(); it != pub_sub_db.end();)
    {
        pub_sub_db_entry_t *pubEntry = it->second;
        auto &subscribers = pubEntry->subscribers;
        subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), SubEntry), subscribers.end());

        // if no subsribers left, delete the map entry
        if (subscribers.empty())
        {
            delete pubEntry;           // delete is used to free the memory, but it is there
            it = pub_sub_db.erase(it); // erase the map entry for unordered_map, it points next one, same for it++
        }
        else
        {
            ++it;
        }
    }
}
/*Display all DB contents */
void coord_db_display()
{
    // printf publishers database
    printf("Publisher DB: \n");
    for (auto it = pub_db.begin(); it != pub_db.end(); it++)
    {
        printf("Publisher ID :%u, Publisher name: %s. \n", it->second->publisher_id, it->second->pub_name);
        /*print published messages*/
        for (int i = 0; i < MAX_PUBLISHED_MSG; i++)
        {
            if (it->second->published_msg_ids[i])
            {
                printf("Published Message ID:%u\n", it->second->published_msg_ids[i]);
            }
        }
    }

    // print subscribers database
    printf("Subscribers ID: \n");
    for (auto it = sub_db.begin(); it != sub_db.end(); it++)
    {
        printf("Subscribers ID: %u,Subscribers name:%s\n", it->second->subsriber_id, it->second->sub_name);
        /*print subscriberd messages*/
        for (int i = 0; i < MAX_SUBSCRIBED_MSG; i++)
        {
            if (it->second->subscriber_msg_ids[i])
            {
                printf("Subscribed Message ID: %u\n", it->second->subscriber_msg_ids[i]);
            }
        }
    }

    // printf message ID and its subscribers name
    printf("Pub_Sub_db:\n");
    for (auto it = pub_sub_db.begin(); it != pub_sub_db.end(); it++)
    {
        // printf("Message ID:%u\n", it->first); //my code
        printf("Messgae ID :%u\n", it->second->publish_msg_code);
        for (auto iter = it->second->subscribers.begin(); iter != it->second->subscribers.end(); iter++)
        {
            // printf("Subscribers name:%s\n", (*iter)->sub_name);
            printf("Subscribers ID:%u\n", (*iter)->subsriber_id);
        }
        // advanced
        /*
        for (const auto &sub : it->second->subscribers)
        {
            std::cout << "Subscribes name:" << sub->sub_name << "\n";
        }*/
    }
}