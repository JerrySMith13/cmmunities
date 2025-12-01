#ifndef MESSAGING_H
#define MESSAGING_H
#include "../global_types.h"

typedef struct Message{
    cid channel_id;
    uid sender;
    char* words;
} Message;

typedef struct Channel{
    gid group;
    cid id;
    char* name;
} Channel;
 
int build_msg(const char* data, Message* surrogate);

// This will return a new, positive integer id that the client can use to identify its message
int push_msg(const Message* to_push, cid channel);

int get_msgs(int start, int num, cid channel, Message* fetched);

int get_msg_direct(int id, cid channel);

/*
All channels have rwx perms for diff user groups
for example, 'all' users could be made default for every user   
*/

#endif
