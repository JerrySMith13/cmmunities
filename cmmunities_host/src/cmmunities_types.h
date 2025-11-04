/*
Types for cmmunities
This file is a standard definition for types that must be shared throughout the project.
*/

#ifndef CMMUNITIES_TYPES_H
#define CMMUNITIES_TYPES_H

#include <stddef.h>

typedef struct { // string
  char* literal;
  size_t len;
} string;

typedef unsigned int uid;
typedef unsigned int gid;
typedef unsigned int channel_id;
#endif
