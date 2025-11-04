#ifndef CHANNELS_H
#define CHANNELS_H

#include <stddef.h> 
#include <time.h>
#include <stdint.h>
#include <sqlite3.h>

#include "../cmmunities_types.h"

typedef uint32_t msg_id;

typedef struct { // Message
  uid sender;
  channel_id sent_to;
  time_t date_sent;
  string contents;
  uint32_t ttl;
  msg_id id;
     
} Message;

Message* init_message(uid sender, channel_id sent_to, string contents, uint32_t ttl);

enum MsgTransaction{
  ADD_MSG,
  DEL_MSG,
  EDIT_MSG  
};



#endif
