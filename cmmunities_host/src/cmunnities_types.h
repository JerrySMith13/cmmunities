#ifndef CMMUNITIES_TYPES_H
#define CMMUNITIES_TYPES_H

#include "stdint.h"
#include "stdbool.h"
#include"assert.h"
#include "stdlib.h"
#include <stdint.h>

#define static_assert _Static_assert

struct id_list{
    uint32_t* head;
    uint32_t length;
};

enum Permissions{
    READONLY,
    BASIC,
    ADMIN,
    SUPERUSER
};


#define CHANNEL_FIELD_COUNT 5
struct Channel{
    uint32_t channel_id;
    char* channel_name;
    bool readonly;
    struct id_list whitelist;
    struct id_list blacklist;
    
};

enum ChannelMembers{
    channel_id,
    channel_name,
    readonly,
    whitelist,
    blacklist,
    CHANNELMEMBERS_NUM_FIELDS
};

static_assert(CHANNEL_FIELD_COUNT == CHANNELMEMBERS_NUM_FIELDS);

#define USER_FIELD_COUNT 3
struct User{
    uint32_t user_id;
    char* username;
    enum Permissions perm_level;

};

enum UserMembers{
    user_id,
    username,
    perm_level,
    USERMEMBERS_NUM_FIELDS,
};

static_assert(USER_FIELD_COUNT == USERMEMBERS_NUM_FIELDS);


#define MESSAGE_NUM_FIELDS 4
struct Message{
    char* contents;
    uint64_t timestamp;
    uint32_t sender;
    uint32_t channel_posted;

};

enum MessageMembers{
    contents,
    timestamp,
    sender,
    channel_posted,
    MESSAGEMEMBERS_NUM_FIELDS,
};

static_assert(MESSAGE_NUM_FIELDS == MESSAGEMEMBERS_NUM_FIELDS);


#endif