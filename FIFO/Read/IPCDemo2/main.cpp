#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>
#include <unistd.h>
#include "errno.h"

int main()
{
    int fd;
    int len;
    char buffer[1024];
    
    if(mkfifo("/tmp/fifol", 0666) < 0 && errno != EEXIST)
        perror("Create FIFO Failed");
    
    if((fd = open("/tmp/fifol", O_RDONLY)) < 0)
    {
        perror("Open FIFO Failed");
        exit(1);
    }
    
    while((len = read(fd, buffer, 1024)) > 0)
        printf("Read message: %s", buffer);
    
    close(fd);
    return 0;
}
