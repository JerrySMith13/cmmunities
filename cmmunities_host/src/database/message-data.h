
#ifndef DATABASE_H
#define DATABASE_H

#include "./messages.h"
#include "../cmmunities_types.h"
#include <cstddef>

enum TransType{
  MSG_ADD,
  MSG_DEL,
  MSG_EDIT
};

typedef union TransactionData{
  // If adding a message
  const Message* to_add;

  // If deleting a message
  msg_id to_delete;

  // If editing a message
  struct MsgEdits{
    string edits;
    msg_id id;
  };
} TransactionData;

typedef struct Transaction{
  const TransactionData* data;
  const TransType type;  
} Transaction;

const Transaction construct_deletion(msg_id to_delete);
const Transaction construct_addition(const Message* to_add);
const Transaction construct_edit(msg_id to_edit, string new_text);

struct QueueNode{
  Transaction trans;
  QueueNode* next;
  bool should_free = true;
};

typedef struct TransQueue{
  QueueNode head;
  QueueNode tail;
  size_t len;
} TransQueue;

const Transaction* TransQueue_pop(TransQueue* self);
void TransQueue_push(TransQueue* self, const Transaction* to_add);



#endif
