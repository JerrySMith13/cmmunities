//Simple implementation of an on-device peer list that lets a user connect to known peers
#include <sys/socket.h>
#include <stdint.h>

struct peer{
  struct sockaddr* address;
  uint8_t mac[6];
  uint8_t hash[32];
}

struct peer_list{
  unsigned int size;
  
}


/*
Each peer is represented by a remote address, MAC address, and key hash (SHA1)
The key hash is needed for most cmmunity operations, and the permissions this key accesses can be changed by committee vote.



*/
