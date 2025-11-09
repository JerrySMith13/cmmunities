/*
entire tree is stored in singular file

An entry is made up of a few entities:
- A name: 20 characters long max, 20 bytes long, may contain any ASCII character
- A primary number, 32 bit number which represents the index of the next file within its own directory (./)
- A secondary number, 32 bit number which represents:
  - The index of the first entry inside of a directory, if the entry is a directory
  - The channel id, if the entry is a channel
  - The index of the entry being pointed to, if the entry is a symlink

Therefore, each entry is 36 bytes large.

The header of the file contains:
- The total number of entries
- The "index" of the beginning of a list of free entries
- The "index" of the end of free entries / end of file

to get file position from an index, just multiply it by the size of an entry and offset it by the size of headers.

- every time we run out of space for new entries, we extend the index of the free entries by a certain amount, and changes the header accordingly


- Typecast all indexes from off_t into uint64_t to ensure portability




Basic structure for headers (header size offset is 8 bytes):

num_entries: u32, 4 bytes long
num_free: u32, 4 bytes long
free_list_len: u32, 4 bytes long

header length is 12 bytes
therefore, position of first unallocated entry (in bytes) is: header length + (num_entries * entry_size)
*/

#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>

#include "cmmunities_types.h"
//for memset
#include <string.h>

#include <errno.h>

#define TREE_HEADER_SIZE 4
#define NUM_HEADERS 3
#define ENTRY_SIZE 36
#define ROOT_INDEX TREE_HEADER_SIZE * NUM_HEADERS

#define ENTRY_NAME_LEN 20


typedef struct Tree{
  int underlying_fd;
  uint64_t len;
  off_t  free_indexes_start;
  off_t free_indexes_len;
  
} Tree;

typedef struct Entry{
  char name[20];
  uint32_t next_ref;
  uint32_t secondary_ref;
} Entry;

static int create_tree(int fd, uint32_t free_index_offset);
int open_tree(string* name, Tree* surrogate);
int close_tree(Tree* to_close);

int read_path(const Tree* tree, string* path, Entry* surrogate);

string** ls(const Tree* tree, string* path);

int mkdir(Tree* tree, string* path, string* name);

int create(Tree* tree, string* name, string* directory);

int link(Tree* tree, string* path_to_link, string* path_to_create_in, string* name);


int open_tree(string* name, Tree* surrogate){
  int fd;
  off_t offset = 0;

  uint64_t len;
  off_t free_indexes_start;
  fd = open(name->literal, O_RDWR | O_EXCL | O_CREAT);
  
  int possible_err;
  if (fd != -1) possible_err = create_tree(fd, 10);
  else fd = open(name->literal, O_RDWR);

  if (fd < 0 || possible_err < 0) return -1; //Error handling needed
  
  uint32_t headers[NUM_HEADERS]; 
  if (lseek(fd, 0, SEEK_SET) == -1) return -1;
  if (read(fd, headers, NUM_HEADERS * sizeof(uint32_t)) == -1) return -1;

  surrogate->underlying_fd = fd;
  surrogate->len = headers[0];
  surrogate->free_indexes_start = headers[1];
  surrogate->free_indexes_len = headers[2];

  return 0;
}

int close_tree(Tree* self){
  close(self->underlying_fd);
}
//ONLY CALLED if the file was JUST opened
// free_index_offset is the index at which the unwritten entries end, and the list of free entries may begin. 
static int create_tree(int fd, uint32_t free_index_offset){

    if (posix_fallocate(fd, 0, (NUM_HEADERS * TREE_HEADER_SIZE) + (free_index_offset * ENTRY_SIZE)) != 0) return -1; //Error handling needed
    off_t offset = 0;
    uint32_t headers[NUM_HEADERS] = {(uint32_t) 0, free_index_offset, (uint32_t) (0)};

    if (lseek(fd, offset, SEEK_SET) == -1) return -1; //error handling needed
    if (write(fd, headers, NUM_HEADERS * sizeof(uint32_t)) == -1) return -1;

    if (lseek(fd, NUM_HEADERS * sizeof(uint32_t), SEEK_SET) == -1) return -1;
    char empty[ENTRY_NAME_LEN];
    memset(empty, 0, ENTRY_NAME_LEN);
    
    uint32_t numbers[2] = {0, 0};
    
    offset = ROOT_INDEX;
    if(lseek(fd, offset, SEEK_SET) == -1) return -1;
    if(write(fd, empty, ENTRY_NAME_LEN) == -1) return -1;

    offset = offset + ENTRY_NAME_LEN;
    if (lseek(fd, offset, SEEK_SET) == -1) return -1;
    if (write(fd, numbers, sizeof(uint32_t) * 2) == -1) return -1;

    return 0;
}

int read_path(const Tree* tree, string* path, Entry* surrogate){
  string** path_split;
  

  for (size_t i, start = path->start; i <= path->len + path->start; i++){
    if (path->literal[i] == '/'){
      
    }
  }
};