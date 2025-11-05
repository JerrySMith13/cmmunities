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
- The index of the beginning of a list of free entries
- The index of the end of free entries / end of file
- The page size (in)

- every time we run out of space for new entries, we extend the index of the free entries by a certain amount, and changes the header accordingly


- Typecast all indexes from off_t into uint64_t to ensure portability
*/

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define _GNU_SOURCE
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
  if (fd != -1) {
    create_tree(fd, 10);
    if (errno != EEXIST) return -1; //Error Handling needed
  }
  else fd = open(name, O_RDWR);
   
  /* CHANGE: Make reading headers one large read and then just deserialize blob into header values, will increase performance*/
  if (lseek(fd, offset, SEEK_SET) == -1) return -1; //Error Handling needed
  if (read(fd, &len, 8) <= 0) return -1; //Error Handling needed
  offset += 8;
  if (lseek(fd, offset, SEEK_SET) == -1) return -1;
  if (read(fd, &free_indexes_start, 8) == -1) return -1;
  *surrogate = (Tree){
    .underlying_fd = fd,
    .len = len,
    .end_of_entries = end_of_entries,
    .free_indexes_start = free_indexes_start,
  };
  return 0;
}

int close_tree(Tree* self){

}
//ONLY CALLED if the file was JUST opened
static int create_tree(int fd, off_t free_index_offset){
    if (posix_fallocate(fd, 0, (NUM_HEADERS * TREE_HEADER_SIZE) + (free_index_offset * ENTRY_SIZE)) != 0) return -1; //Error handling needed
    
    uint64_t num_entries = 0;
    uint64_t free_index_offset = (uint64_t) free_index_offset;
    uint64_t 

    //Write num_entries
    if (lseek(fd, 0, SEEK_SET) == -1) return -1; //Error handling needed
    if (write(fd, (uint64_t) 0, sizeof(uint64_t)) == -1) return -1;

    if (lseek(fd, sizeof(uint64_t), SEEK_SET) == -1) return -1;
    if (write(fd, (uint64_t) free_index_offset, sizeof(uint64_t))) return -1; 




}
