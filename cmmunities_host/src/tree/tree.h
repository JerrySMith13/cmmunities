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
#include <fcntl.h>
#include <unistd.h>

#define TREE_HEADER_SIZE ;
#define ENTRY_SIZE 36

Tree open_tree(char* name){
  int underlying_fd;
  char buffer[36];
  ssize_t bytes_read;
  
  int fd = open(name, O_RDWR | O_EXCL | O_CREAT);
  
  
}

typedef struct Tree{
  int underlying_fd;
  size_t len;
  size_t end_of_entries;
  size_t free_indexes_start;
  
} Tree;
