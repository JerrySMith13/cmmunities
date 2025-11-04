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
- The index of the end of entries
- The index of the beginning of free entries
-every time we run out of space for new entries, we extend the index of the free entries by a certain amount, and changes the header accordingly

*/

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>

#define TREE_HEADER_SIZE 8;
#define ENTRY_SIZE 36

int open_tree(char* name, Tree* surrogate){
  int fd;
  off_t offset = 0;

  uint64_t len, end_of_entries, free_indexes_start;
  fd = open(name, O_RDWR | O_EXCL | O_CREAT);
  if (fd != -1) {
    create_tree(fd, 10);
  }
  else if (errno != EEXIST) return -1; //Error Handling needed

  if (lseek(fd, offset, SEEK_SET) == -1) return -1; //Error Handling needed
  if (read(fd, &len, 8) <= 0) return -1; //Error Handling needed
  offset += 8;
  if (lseek(fd, offset, SEEK_SET) == -1) return -1;
  if (read(fd, &end_of_entries, 8) == -1) return -1;
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
//ONLY CALLED if the file was JUST opened
static void create_tree(int fd, size_t free_index_offset){
  
}

typedef struct Tree{
  int underlying_fd;
  uint64_t len;
  uint64_t end_of_entries;
  uint64_t free_indexes_start;
  
} Tree;
