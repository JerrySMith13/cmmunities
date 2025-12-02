#ifndef USERS_H
#define USERS_H

#include "../global_types.h"
#include <fcntl.h>

typedef struct User{
    char* name;
    uid id;
    gid* gid_list;
    gid primary;
} User;

typedef struct Group{
    char* name;
    gid id;
    uid* users;
} Group;

int get_user(uid id, User* surrogate);
int useradd(char* name, gid primary, gid* gid_list);
int userdel(uid id);

int get_group(gid id, Group* surrogate);
int groupadd(char* name, uid* to_add);
int groupdel(gid to_del);

off_t seek_mid_line(int fd, off_t fsize);


#endif
