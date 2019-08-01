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
    
    msg.myType = 888; //client send msg type
    sprintf(msg.myText, "Hello I am Client %d", getpid());
    msgsnd(msqid, &msg, sizeof(msg.myText), 0);
    
    msgrcv(msqid, &msg, 256, 999, 0); // client receive msg type
    printf("Client: receive msg.text is: %s", msg.myText);
    printf("Client: receive msg.type is: %ld", msg.myType);
    return 0;
}

