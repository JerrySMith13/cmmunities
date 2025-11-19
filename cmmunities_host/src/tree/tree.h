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
  enum EntryType type;
} Entry;

//ONLY CALLED if the file was JUST opened
// free_index_offset is the index at which the unwritten entries end, and the list of free entries may begin. 
int create_tree(int fd, uint32_t free_index_offset);

int open_tree(const char* name, Tree* surrogate);

//Still must deallocate tree if it was manually allocated
int close_tree(Tree* to_close);

int read_path(const Tree* tree, char* path, Entry* surrogate);

char** ls_tree(const Tree* tree, const char* path);

int makedir_tree(Tree* tree, const char* path, const char* name);

int link_tree(Tree* tree, const char* path_to_link, const char* path_to_create_in, const char* name);

int read_index(uint32_t index);

int read_tree_direct(const char* path);
