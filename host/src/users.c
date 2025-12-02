#include "../headers/users.h"
#include "../headers/conf.h"


#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h> 

static int advance_line(int fd, char buf[20]);

enum Userr{
  Ok = 1,
  FileErr,
  SeekErr,
  Empty,

};

int get_user(uid id, User* surrogate){
  struct stat statbuf;
  int fd;
  off_t fsize;

  char buf[20];
  
  if (stat(globalConf->cmmunity_userdata, &statbuf) == -1){
    return FileErr;
  }
  fsize = statbuf.st_size;
  off_t pos = fsize/2;
  

  fd = open(globalConf->cmmunity_userdata, O_RDONLY);
  if (fd < 0){
    return FileErr;
  }
  
  
}
// This returns the position of the middle line, as a file offset
off_t seek_mid_line(int fd, off_t fsize){
  
  char buf[20];
  off_t pos = fsize/2;

  bool seek_flag;
  uint8_t bytes_read = pread(fd, &buf, 20, pos);
  if (bytes_read < 0) return Empty;
  if (bytes_read == 0) return -1;

  char* newline = strchr(buf, '\n');
  
  if (newline != NULL) {
    
    for (int i = 0; i < strlen(newline); i++){
      if (newline[i] == '\n'){
        return (off_t) (newline - buf) + i; 
      };
    }

    return SeekErr;
  }
  // This triggers if there is only one line in the file, thus making 0 the offset for it.
  if (bytes_read < 20) return 0;

  //Begin advancing by one to find it ahead of us
  off_t mid_pos = fsize + bytes_read + 1;
  char minibuf[5];
  for (off_t i = mid_pos; i < fsize; i+=5){
    bytes_read = pread(fd, buf, 5, i);

    if (bytes_read < 0){
      return FileErr;
    }
    
    // Basically tells us to start from the beginning, file is short enough (complicated logic, but it covers all paths even if starting from 0 instead of middle)
    if (bytes_read < 5){
      newline = strchr(minibuf, '\n');
      if (newline == NULL) return 0;
      else return i + (newline - minibuf);
    }

    newline = strchr(minibuf, '\n');
    if (newline == NULL) continue;
    else return mid_pos + i + (newline - minibuf);

  }
}