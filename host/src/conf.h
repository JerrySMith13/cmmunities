#ifndef CONF_H
#define CONF_H
/*
To save time, cache a built file that can be directly cast to C struct with a hash of conf file, when changed rebuild the file



*/

#include <stdlib.h>
typedef struct ConfObject{
    char* cmmunity_root; //Represents cmmunity root dir, where all database files and associated config is stored
} ConfObject;


// WILL ALLOCATE FOR YOU, JUST NEED TO DEALLOC
int find_conf(char* path_to_write);

int build_conf(char* path); 

static ConfObject* globalConf = NULL; 

#endif