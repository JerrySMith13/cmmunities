#include "../headers/users.h"
#include "../headers/conf.h"


#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>


enum Userr{
  FileErr
};

int get_user(uid id, User* surrogate){
  struct stat statbuf;
  int fd;
  off_t fsize;
  
  if (stat(globalConf->cmmunity_userdata, &statbuf) == -1){
    return FileErr;
  }
  fsize = statbuf.st_size;
  off_t pos = fsize/2;
  

  fd = open(globalConf->cmmunity_userdata, O_RDONLY);

  while (true){
    
  }

  

  

}
  
