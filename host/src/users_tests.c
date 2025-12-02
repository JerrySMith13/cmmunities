#include <fcntl.h>
#include "../headers/users.h"
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
int main(void){
    int fd = open("test.txt", O_RDONLY);
    if (fd < 0) printf("Error is here 8 "); return -1;
    struct stat w;
    if (fstat(fd, &w) < 0) printf("Error is here 10"); return -1;
    off_t middle = seek_mid_line(fd, w.st_size);
    char buf[20];
    if (pread(fd, buf, 20, middle) < 0) printf("Error is here 13"); return -1;

    char* whataber = strchr(buf, '\n');
    if (whataber != NULL){
        buf[whataber - buf] = '\0';
    }
    printf("Middle line: %s", buf);

}