/*
entire tree is stored in singular file

An entry is made up of a few entities:
- A 16-bit number representing the type of entry
- A name: 20 characters long max, 20 bytes long, may contain any ASCII character
- A primary number, 32 bit number which represents the index of the next file within its own directory (./)
- A secondary number, 32 bit number which represents:
  - The index of the first entry inside of a directory, if the entry is a directory
  - The channel id, if the entry is a channel
  - The index of the entry being pointed to, if the entry is a symlink

we then are left with 10 extra bytes (both for extensions if needed and for padding to conform with block sizes, usually 4 kb)

The header of the file contains:
- The total number of entries
- The "index" of the beginning of a list of free entries
- The "index" of the end of free entries / end of file

to get file position from an index, just multiply it by the size of an entry and offset it by the size of headers.

- every time we run out of space for new entries, we extend the index of the free entries by a certain amount, and changes the header accordingly


- Typecast all indexes from off_t into uint64_t to ensure portability

HEADERS (12 bytes) | name (20 bytes), next_entry (4 bytes), magic_number (4 bytes) | ... | free_space(28 bytes) | ... |


Basic structure for headers (header size offset is 12 bytes):

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

#include "../cmmunities_types.h"
//for memset
#include <string.h>

#include <errno.h>

#define findex_from(index) (off_t) (NUM_HEADERS * TREE_HEADER_SIZE) + (ENTRY_SIZE * index);

#define TREE_HEADER_SIZE 4
#define NUM_HEADERS 3
#define ENTRY_SIZE 40
#define ROOT_INDEX TREE_HEADER_SIZE * NUM_HEADERS

#define ENTRY_NAME_LEN 20


typedef struct Tree{
  int underlying_fd;
  uint32_t len;
  uint32_t  free_indexes_start;
  uint32_t free_indexes_len;
  
} Tree;

enum EntryType{
  CHANNEL,
  DIR,
  LINK,
  EMPTY
};

typedef struct Entry{
  char name[20];
  uint32_t next_ref;
  uint32_t secondary_ref;
  EntryType type;
} Entry;


static int create_tree(int fd, uint32_t free_index_offset);
int open_tree(const char* name, Tree* surrogate);
int close_tree(Tree* to_close);

int read_path(const Tree* tree, char* path, Entry* surrogate);

char** ls(const Tree* tree, const char* path);

int mkdir(Tree* tree, const char* path, const char* name);

int create(Tree* tree, const char* name, const char* directory);

int link(Tree* tree, const char* path_to_link, const char* path_to_create_in, const char* name);

int read_index(uint32_t index);

int open_tree(const char* name, Tree* surrogate){
  int fd;
  off_t offset = 0;

  uint32_t len;
  uint32_t free_indexes_start;
  fd = open(name, O_RDWR | O_EXCL | O_CREAT);
  
  int possible_err;
  if (fd != -1) possible_err = create_tree(fd, 10);
  else fd = open(name, O_RDWR);

  if (fd < 0 || possible_err < 0) return -1; //Error handling needed
  
  uint32_t headers[NUM_HEADERS]; 
  if (pread(fd, headers, NUM_HEADERS * sizeof(uint32_t), 0) == -1) return -1;

  surrogate->underlying_fd = fd;
  surrogate->len = headers[0];
  surrogate->free_indexes_start = headers[1];
  surrogate->free_indexes_len = headers[2];

  return 0;
}

// Still must deallocate tree if it was dynamically allocated
int close_tree(Tree* self){
  close(self->underlying_fd);
}
//ONLY CALLED if the file was JUST opened
// free_index_offset is the index at which the unwritten entries end, and the list of free entries may begin. 
static int create_tree(int fd, uint32_t free_index_offset){

    //Step 1: Allocate necessary bytes for file
    if (posix_fallocate(fd, 0, (NUM_HEADERS * TREE_HEADER_SIZE) + (free_index_offset * ENTRY_SIZE)) != 0) return -1; //Error handling needed

    //Step 2: Create offset to write to, plus init headers to write
    off_t offset = 0;
    uint32_t headers[NUM_HEADERS] = {(uint32_t) 0, free_index_offset, (uint32_t) (0)};

    //Step 3: write headers to file
    if (pwrite(fd, headers, NUM_HEADERS * sizeof(uint32_t), offset) == -1) return -1;

    //Create empty name buffer + null values for both next_dir value and magic number, and a special "root type" for entry. This entry is overwritten by the first created entry in the tree.
    uint16_t empty_t = (uint16_t) EMPTY;
    char empty[ENTRY_NAME_LEN];
    memset(empty, 0, ENTRY_NAME_LEN);
    uint32_t numbers[2] = {0, 0};

    // Set offset to root index, write empty name to that 
    offset = ROOT_INDEX;
    
    if(pwrite(fd, &empty_t, sizeof(empty_t), offset) == -1) return -1;
    offset += sizeof(empty_t);

    if (pwrite(fd, &empty, ENTRY_NAME_LEN, offset) == -1) return -1;
    offset += ENTRY_NAME_LEN;
    
    if (pwrite(fd, numbers, sizeof(uint32_t) * 2, offset) == -1) return -1;

    return 0;
}

static int read_tree_direct(const char* path){
  int fd = open(path, O_RDONLY);
  off_t offset = 0;
  uint32_t headers[3];
  if (pread(fd, &headers, sizeof(uint32_t) * 3, offset) == -1) return -1;
  printf("Number of entries: %d\n\
          Free data past last entry: %d\n\
          Number of free entry indices: %d",
        headers[0], headers[1] - headers[0], headers[2] - headers[1]);

  for(int i = 0; i < headers[0]; i++){
    off_t index = findex_from(i);
    uint16_t type_n;
    enum EntryType type;
    
    if (pread(fd, &type_n, sizeof(uint16_t), index) == -1) return -1;
    type = (enum EntryType) type_n;

    index += sizeof(uint16_t);
    char name[21];
    if (pread(fd, &name, ENTRY_NAME_LEN, index) == -1) return -1;
    name[20] = '\0';

    index += ENTRY_NAME_LEN;
    uint32_t numbers[2];
    if (pread(fd, &numbers, sizeof(uint32_t) * 2, index) == -1) return -1;

    char entry_type[4];

    switch (type){
      case EMPTY:
        strncpy(entry_type, "NUL", 3);
      case CHANNEL:
        strncpy(entry_type, "CHN", 3);
      case DIR:
        strncpy(entry_type, "DIR", 3);
      case LINK:
        strncpy(entry_type, "LNK", 3);
    }

    entry_type[3] = '\0';

    printf("\n---- %s ----\n\
           Type: %s\n\
           Next entry: %d\n\
           Magic num: %d",
           name, entry_type, numbers[0], numbers[1]);
    return 0;
    
  }

}



