#include "stdint.h"

#ifndef CMMUNITIES_TYPES_H
#define CMMUNITIES_TYPES_H
enum Permissions{
    READONLY,
    BASIC,
    ADMIN,
    SUPERUSER
};

typedef uint32_t userid;
typedef uint32_t* id_list;

struct Channel{
    id_list grey_list;
    char* channel_name;
    bool exclusive;
    id_list writers;
};

struct User{
    uint32_t id;
    char* username;
    enum Permissions perm_level;

};

struct Message{
    char* contents;
    uint64_t timestamp;
    userid sender;
    char* sent_to;

};

#endif