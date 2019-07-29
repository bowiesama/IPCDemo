#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>
#include <unistd.h>

int main()
{
    int fd;
    int n, i;
    char buffer[1024];
    time_t tp;
    
    printf("I am %d process. \n", getpid());
    if((fd = open("/tmp/fifol", O_WRONLY)) < 0)
    {
        perror("Open FIFO Failed");
        exit(1);
    }
    
    for(i = 0; i < 10; i++)
    {
        time(&tp);
        n = sprintf(buffer, "Process %d's time is %s", getpid(), ctime(&tp));
        printf("Send message : %s", buffer);
        if(write(fd, buffer, n + 1) < 0)
        {
            perror("Write FIFO Failed");
            close(fd);
            exit(1);
        }
        sleep(1);
    }
    close(fd);
    return 0;
}
