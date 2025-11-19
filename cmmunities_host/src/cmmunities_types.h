/*
Types for cmmunities
This file is a standard definition for types that must be shared throughout the project.
*/

#ifndef CMMUNITIES_TYPES_H
#define CMMUNITIES_TYPES_H


//Null-terminated
char** string_split(char* str, char split_by);
void string_split_free(char** split);


typedef unsigned int uid;
typedef unsigned int gid;
typedef unsigned int channel_id;
#endif
