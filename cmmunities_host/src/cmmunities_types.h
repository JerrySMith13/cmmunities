/*
Types for cmmunities
This file is a standard definition for types that must be shared throughout the project.
*/

#ifndef CMMUNITIES_TYPES_H
#define CMMUNITIES_TYPES_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct { // string
  char* literal;
  size_t len;
  size_t start;
} string;

string* string_split(string* to_split, char character){
  size_t len = 0;
  for (size_t i = to_split->start; i < to_split->start + to_split->len; i++){
    if (to_split->literal[i] == character) len++;
  }

  string* list = (string*) malloc(len * sizeof(string));

  size_t begin_index = to_split->start;
  size_t current_pos = 0;
  for (size_t i = to_split->start; i < to_split->start + to_split->len; i++){
    if (to_split->literal[i] == character) {
      string fart = (string) {
        .literal = to_split->literal,
        .len = i - begin_index,
        .start = begin_index,
      };
      begin_index = i;
      list[current_pos] = fart;

    };
  }

  return list;

}

string slice(const string* orig, size_t begin_index, size_t len){
  return (string) {
    .literal = orig->literal,
    .len = len,
    .start = begin_index,
  };
}

typedef unsigned int uid;
typedef unsigned int gid;
typedef unsigned int channel_id;
#endif
