#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_FILE "/tmp/README"  //create unique key for msg queue

//msg struct
struct msg_form {
    long myType;
    char myText[256];
};

int main()
{
    int msqid;
    key_t key;
    struct msg_form msg;
    
    //get key
    if((key = ftok(MSG_FILE, 'z')) < 0)
    {
        perror("ftok error");
        exit(1);
    }
    printf("Message Queue - Server key is: %d. \n", key);
    
    //create msg queue
    if((msqid = msgget(key, IPC_CREAT|0777)) == -1)
    {
        perror("MSGET ERROR");
        exit(1);
    }
    
    printf("My msqid is %d. \n", msqid);
    printf("My pid id %d. \n", getpid());
    
    // read msg
    for(;;)
    {
        msgrcv(msqid, &msg, 256, 888, 0); // return msg which type is 888
        printf("Server: receive msg.MyText is %s. \n", msg.myText);
        printf("Server: receive msg.MyType is %ld. \n", msg.myType);
        
        msg.myType = 999; //server send msg type
        sprintf(msg.myText, "Hello I am Server %d", getpid());
        msgsnd(msqid, &msg, sizeof(msg.myText), 0);
    }
    return 0;
}

