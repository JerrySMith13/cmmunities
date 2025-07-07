#ifndef CMMUNITIES_TYPES_H
#define CMMUNITIES_TYPES_H

#include "stdint.h"
#include "stdbool.h"
#include"assert.h"
#include "stdlib.h"

#define static_assert _Static_assert
enum Permissions{
    READONLY,
    BASIC,
    ADMIN,
    SUPERUSER
};

typedef uint32_t userid;
typedef uint32_t* id_list;
typedef uint32_t channelid;

#define CHANNEL_FIELD_COUNT 5
struct Channel{
    channelid id;
    id_list grey_list;
    char* channel_name;
    bool is_exclusive;
    id_list writers;
    
};

enum ChannelMembers{
    id,
    grey_list,
    channel_name,
    is_exclusive,
    writers,
    CHANNELMEMBERS_NUM_FIELDS
};

static_assert(CHANNEL_FIELD_COUNT == CHANNELMEMBERS_NUM_FIELDS);

#define USER_FIELD_COUNT 3
struct User{
    uint32_t id;
    char* username;
    enum Permissions perm_level;

};

enum UserMembers{
    id,
    username,
    perm_level,
    USERMEMBERS_NUM_FIELDS,
};

static_assert(USER_FIELD_COUNT == USERMEMBERS_NUM_FIELDS);


#define MESSAGE_NUM_FIELDS 4
struct Message{
    char* contents;
    uint64_t timestamp;
    userid sender;
    char* sent_to;

};

enum MessageMembers{
    contents,
    timestamp,
    sender,
    sent_to,
    MESSAGEMEMBERS_NUM_FIELDS,
};

static_assert(MESSAGE_NUM_FIELDS == MESSAGEMEMBERS_NUM_FIELDS);


#endif