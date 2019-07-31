#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>
#include <unistd.h>
#include "errno.h"

int main()
{
    int fd[2];
    pid_t pid;
    char buffer[20];
    
    if(pipe(fd) < 0) // create pipe
        printf("Create pipe Failed\n");
    if((pid = fork()) < 0)  //create child process
        printf("Fork error.\n");
    else if(pid > 0)
    {
        close(fd[0]); //close read
        write(fd[1], "hello world\n", 12);
    }
    else
    {
        close(fd[1]); // close write
        read(fd[0], buffer, 20);
        printf("%s", buffer);
    }
    return 0;
}
