#include "stdlib.h"
#include "stdint.h"
#include <string.h>

#include "sqlite3.h"

#include "cmunnities_types.h"

#include "log.h"



#define CHANNEL_DB_NAME "cmmunities_channels.db"
#define CHANNEL_INFO_TABLE_NAME "cmmunities_channels_info"

uint32_t num_channels(sqlite3* channel_db){
    const char* query = "SELECT COUNT(channel_name) FROM cmmunities_channels_info;";
    sqlite3_stmt *stmt;
    int count = -1;

    if (sqlite3_prepare_v2(channel_db, query, -1, &stmt, NULL) == SQLITE_OK){
        if (sqlite3_step(stmt) == SQLITE_ROW){
            count = sqlite3_column_int(stmt, 0);
        }
        else {
            log("Error originating from line 20 of channels.c: row count couldn't be retrieved");
        }
    }
    else{
        logf("Error in channels.c: %s", sqlite3_errmsg(channel_db));
    }

    sqlite3_finalize(stmt);
    return count;
}

uint32_t list_channels(sqlite3* channel_db, char** buf){
    const char* query = "SELECT channel_name FROM cmmunities_channels_info;";
    sqlite3_stmt* stmt;

    int count = num_channels(channel_db);

    if (count < 0){
        log("Error getting number of channels in list_channels");
        return -1;
    }
    
    if (sqlite3_prepare_v2(channel_db, query, -1, &stmt, NULL) != SQLITE_OK){
        logf("Failed to prepare statement for channel info: %s", sqlite3_errmsg(channel_db));
        return -1;
    }

    buf = (char**) malloc(sizeof(char*) * count);
    uint32_t i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW){
        const unsigned char* name = sqlite3_column_text(stmt, 0);
        buf[i] = malloc(strlen(name) * sizeof(char)) + 1;
        strcpy(name, buf[i]);
        i++;
    }
    return i + 1;
}

uint32_t insert_channel(sqlite3* channel_db, const struct Channel* to_insert){
    const char* query = "INSERT INTO cmmunities_channel_info VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(channel_db, query, -1, &stmt, 0) != SQLITE_OK){
        logf("Failed to prepare statement: %s", sqlite3_errmsg(channel_db));
        return -1;
    }

    


}

int get_channel(sqlite3* channel_db, struct Channel* placeholder, const char* name){

    char* query = "SELECT * FROM cmmunities_channel_info WHERE channel_name = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(channel_db, query, -1, &stmt, NULL) == SQLITE_OK){
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

        int counter = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW){
            switch (counter){
                
                
            }
        }
    }

};

void free_str_list(char** list, uint32_t len){
    for (uint32_t i = 0; i < len; i++){
        free(list[i]);
    }
    free(list);
}
