#ifndef CHANNELS_H
#define CHANNELS_H

#include "cmunnities_types.h"
#include "sqlite3.h"

#define CHANNEL_DB_NAME "cmmunities_channels.db"

// Returns total number of channels in community
uint32_t num_channels(sqlite3* channel_db); 

// Returns number of channels listed, allocates buf and fills with list of all channels and their names
uint32_t list_channels(sqlite3* channel_db, char** buf); 

// Returns false if channel does not exist
int get_channel(sqlite3* db, struct Channel* placeholder, const char* name);



int insert_channel(sqlite3* db, const struct Channel* to_insert)


// Loads specified number of messages, returns the number of messages successfully loaded
uint32_t load_msgs(uint32_t len, struct Message* list, const struct Channel* channel);

static void load_channel_into_query(sqlite3_stmt* stmt, )

// Pushes message to database, returns false if it could not push it
bool push_msg(const struct Message* message);

#include "channels.c"

#endif