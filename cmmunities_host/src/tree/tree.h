/*
entire tree is stored in singular file

An entry is made up of a few entities:
- A name: 20 characters long max, 20 bytes long, may contain any ASCII character
- A primary number, 64 bit number which represents the index of the next file within its own directory (./)
- A secondary number, 64 bit number which represents:
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

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>

#include <errno.h>

#define TREE_HEADER_SIZE 8
#define NUM_HEADERS 4
#define ENTRY_SIZE 36
#define FIRST_ROOT_INDEX TREE_HEADER_SIZE * NUM_HEADERS


typedef struct Tree{
  int underlying_fd;
  uint64_t len;
  off_t  free_indexes_start;
  off_t free_indexes_end;
  
} Tree;

static int create_tree(int fd, off_t free_index_offset);
int open_tree(char* name, Tree* surrogate);
int close_tree(Tree* to_close);

int open_tree(char* name, Tree* surrogate){
  int fd;
  off_t offset = 0;

  uint64_t len;
  off_t free_indexes_start;
  fd = open(name, O_RDWR | O_EXCL | O_CREAT);
  
  if (fd != -1) create_tree(fd, 10);
  else fd = open(name, O_RDWR);

  if (fd == -1) return -1; //Error handling needed


  
}

int close_tree(Tree* self){

}
//ONLY CALLED if the file was JUST opened
static int create_tree(int fd, off_t free_index_offset){
    if (posix_fallocate(fd, 0, (NUM_HEADERS * TREE_HEADER_SIZE) + (free_index_offset * ENTRY_SIZE)) != 0) return -1; //Error handling needed
    


}
