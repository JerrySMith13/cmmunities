#include "tree.h"

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

int close_tree(Tree* self){
  close(self->underlying_fd);
  return 0;
}

int create_tree(int fd, uint32_t free_index_offset){

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

int read_tree_direct(const char* path){
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
  }
  
  return 0;
}



