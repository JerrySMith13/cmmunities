#ifndef USERS_H
#define USERS_H

#include "global_types.h"

typedef struct User{
    char* name;
    uid id;
    gid* gid_list;
} User;

int get_user(uid id, User* surrogate);


#endif