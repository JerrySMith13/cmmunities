#include "stdlib.h"
#include "stdint.h"
#include <string.h>

#include "sqlite3.h"

#include "cmunnities_types.h"

#include "log.h"



#define CHANNEL_DB_NAME "cmmunities_channels.db"
#define CHANNEL_INFO_TABLE_NAME "cmmunities_channels_info"
#define CHANNEL_BLACKLIST_TABLE_NAME "cmmunities_blacklist_info"
#define CHANNEL_WHITELIST_TABLE_NAME "cmmunities_whitelist_info"

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

static int insert_into_list(sqlite3* db, uint32_t channel_id, const struct id_list* list, bool is_blacklist){
    sqlite3_stmt* stmt;
    char* query;
    if (is_blacklist) query = "INSERT INTO cmmunities_blacklist_info (channel_id, user_id) VALUES (?, ?);";
    else query = "INSERT INTO cmmunities_whitelist_info (channel_id, user_id) VALUES (?, ?);";

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK){
        logf("Failed to prepare insert when adding user list to table in channels.c: %s", sqlite3_errmsg(db));
        return rc;
    }

    for(uint32_t i = 0; i < list->length; i++){
        sqlite3_bind_int(stmt, 1, channel_id);
        sqlite3_bind_int(stmt, 2, list->head[i]);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE){
            logf("Insert failed at index %d when performing list insertion: %s", i, sqlite3_errmsg(db));
            return rc;
        }

        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;

}

static bool channel_exists(sqlite3* channel_db, const struct Channel* ){

}


//SHOULD NOT BE USED DIRECTLY, instead use 
static uint32_t insert_channel_into_db(sqlite3* channel_db, const struct Channel* to_insert){
    const char* query = "INSERT INTO cmmunities_channel_info VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(channel_db, query, -1, &stmt, 0) != SQLITE_OK){
        logf("Failed to prepare statement: %s", sqlite3_errmsg(channel_db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, to_insert->channel_id);
    sqlite3_bind_text(stmt, 2, to_insert->channel_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, to_insert->readonly);
    
    int rc = insert_into_list(channel_db, to_insert->channel_id, &to_insert->blacklist, true);
    if (rc != SQLITE_OK){
        log("Error inserting into list, aborting operation");
        return rc;
    }

    rc = insert_into_list(channel_db, to_insert->channel_id, &to_insert->whitelist, false);
    if (rc != SQLITE_OK){
        log("Error inserting into list, aborting operation");
        return rc;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE){
        logf("Error inserting channel #%d into database: %s", to_insert->channel_id, sqlite3_errmsg(channel_db));
        return rc;
    }

    sqlite3_finalize(stmt);

    char* create_msg_table_query = "CREATE TABLE ? ( \
        contents TEXT,  \
        timestamp INTEGER, \
        sender INTEGER, \
        channel_posted INTEGER \
    );";

    sqlite3_stmt* message_table_statement;

    sqlite3_prepare_v2(channel_db, message_table_statement, -1, &message_table_statement, NULL);
    sqlite3_bind_text(message_table_statement, 1, create_msg_table_query, -1, SQLITE_STATIC);
    if (sqlite3_step(message_table_statement) != SQLITE_DONE){
        logf("Error creating message database for channel %s: %s", to_insert->channel_name, sqlite3_errmsg(channel_db));
        return -1;
    }
    sqlite3_finalize(message_table_statement);

    return SQLITE_OK;
}




static void fill_channel_blacklist(sqlite3 *db, struct Channel *placeholder)
{

    uint32_t blacklist_count = placeholder->blacklist.length;

    if (blacklist_count != 0) {

        placeholder->blacklist.head = malloc(sizeof(uint32_t) * blacklist_count);

        char *query = "SELECT user_id FROM cmmunities_blacklist_info WHERE channel_id = ?";
        sqlite3_stmt *get_blacklist;

        if (sqlite3_prepare_v2(db, query, -1, &get_blacklist, NULL) != SQLITE_OK)
        {
            logf("Prepare failed in channels.c: %s", sqlite3_errmsg(db));
            return -1;
        };
        sqlite3_bind_int(get_blacklist, 1, placeholder->channel_id);

        uint32_t count = 0;

        while (sqlite3_step(get_blacklist) == SQLITE_ROW)
        {
            if (count > blacklist_count)
            {
                logf("Error: more items in blacklist query for %s than items in blacklist", placeholder->channel_name);
            }

            placeholder->blacklist.head[count] = sqlite3_column_int(get_blacklist, 0);
            count++;
        }
    }

    else
    {
        placeholder->blacklist.head = NULL;
    }
}

static void fill_channel_whitelist(sqlite3 *db, struct Channel *placeholder)
{

    uint32_t whitelist_count = placeholder->whitelist.length;

    if (whitelist_count != 0) {

        placeholder->whitelist.head = malloc(sizeof(uint32_t) * whitelist_count);

        char *query = "SELECT user_id FROM cmmunities_whitelist_info WHERE channel_id = ?";
        sqlite3_stmt *get_whitelist;

        if (sqlite3_prepare_v2(db, query, -1, &get_whitelist, NULL) != SQLITE_OK)
        {
            logf("Prepare failed in channels.c: %s", sqlite3_errmsg(db));
            return -1;
        };
        sqlite3_bind_int(get_whitelist, 1, placeholder->channel_id);

        uint32_t count = 0;

        while (sqlite3_step(get_whitelist) == SQLITE_ROW)
        {
            if (count > whitelist_count)
            {
                logf("Error: more items in whitelist query for %s than items in whitelist", placeholder->channel_name);
            }

            placeholder->whitelist.head[count] = sqlite3_column_int(get_whitelist, 0);
            count++;
        }
    }

    else
    {
        placeholder->whitelist.head = NULL;
    }
}

uint32_t get_channel(sqlite3* channel_db, struct Channel* placeholder, const char* name){

    char* query = "SELECT * FROM cmmunities_channel_info WHERE channel_name = ?;";
    sqlite3_stmt* stmt;

    placeholder = malloc(sizeof(struct Channel));

    if (sqlite3_prepare_v2(channel_db, query, -1, &stmt, NULL) == SQLITE_OK){
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

        for (uint8_t i = 0; i < 3; i++){
            switch(i){
                case channel_id: placeholder->channel_id = sqlite3_column_int(stmt, i); break;
                case channel_name: 
                    const unsigned char* name = sqlite3_column_text(stmt, i);
                    placeholder->channel_name = malloc(strlen(name) + (sizeof(char) + 1));
                    strcpy(placeholder->channel_name, name);
                    break;
                case readonly:
                    placeholder->readonly = sqlite3_column_int(stmt, i);
                    break;
                default:
                    log("Error: too many fields in Channel database");
                    return -1;
            }
        }

        char* len_list_query = "SELECT COUNT(user_id) FROM ? WHERE channel_id= ?;";


        //Get whitelist and blacklist count
        sqlite3_stmt* list_stmt;
        sqlite3_prepare_v2(channel_db, len_list_query, -1, &list_stmt, NULL);

        sqlite3_bind_text(list_stmt, 1, "cmmunities_blacklist_info", -1, SQLITE_STATIC);
        sqlite3_bind_int(list_stmt, 2, placeholder->channel_id);

        int blacklist_count = -1;
        if (sqlite3_step(list_stmt) == SQLITE_ROW){
            blacklist_count = sqlite3_column_int(stmt, 0);
        } 
        
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);

        //Get whitelist count
        sqlite3_bind_text(list_stmt, 1, "cmmunities_blacklist_info", -1, SQLITE_STATIC);
        sqlite3_bind_int(list_stmt, 2, placeholder->channel_id);

        int whitelist_count = -1;
        if (sqlite3_step(list_stmt) == SQLITE_ROW){
            whitelist_count = sqlite3_column_int(stmt, 0);
        } 

        if(whitelist_count < 0 || blacklist_count < 0){
            logf("Error: couldn't retrieve length of whitelist or blacklist for channel: %s", channel_name);
            return -1;
        }
        
        placeholder->blacklist.length = blacklist_count;
        placeholder->whitelist.length = whitelist_count;

        fill_channel_blacklist(channel_db, placeholder);
        fill_channel_whitelist(channel_db, placeholder);
    }
}



void free_str_list(char** list, uint32_t len){
    for (uint32_t i = 0; i < len; i++){
        free(list[i]);
    }
    free(list);
}
