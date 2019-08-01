#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h> // shared memory
#include<sys/sem.h> // semaphore
#include<sys/msg.h> //message queue
#include<string> //memcpy

//message queue
struct msg_form
{
    long myType;
    char myText;
};

//for semctl init
union semunion
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

//init sem
int init_sem(int sem_id, int value)
{
    union semunion tmp;
    tmp.val = value;
    if(semctl(sem_id, 0, SETVAL, tmp) == -1)
    {
        perror("Init Semaphore Error.");
        return -1;
    }
    return 0;
}

//P Operation
//if sem = 1, get resource and sem - 1
//if sem = 0, process wait
int sem_p(int sem_id)
{
    struct sembuf sbuf;
    sbuf.sem_num = 0; //id
    sbuf.sem_op = -1; // P Operation
    sbuf.sem_flg = SEM_UNDO;
    
    if(semop(sem_id, &sbuf, 1) == -1)
    {
        perror("P Operation Error");
        return -1;
    }
    return 0;
}

//V Operation
//release resource and sem + 1
//if process wait, wake up them.
int sem_v(int sem_id)
{
    struct sembuf sbuf;
    sbuf.sem_num = 0; //id
    sbuf.sem_op = 1; // P Operation
    sbuf.sem_flg = SEM_UNDO;
    
    if(semop(sem_id, &sbuf, 1) == -1)
    {
        perror("V Operation Error");
        return -1;
    }
    return 0;
}

int main()
{
    key_t key;
    int shmid, semid, msqid;
    char *shm; // shm handler address
    struct msg_form msg;
    int flag = 1;
    
    //get key
    if((key = ftok("/tmp/README", 'z')) < 0)
    {
        perror("ftok error.");
        exit(1);
    }
    
    //get memory
    shmid = shmget(key, 1024, 0);
    if(shmid == -1)
    {
        perror("shmget error.");
        exit(1);
    }
    
    //link memory
    shm = (char*)shmat(shmid, 0, 0);
    
    //create msg queue
    if((msqid = msgget(key, 0)) == -1)
    {
        perror("msgget error.");
        exit(1);
    }
    
    //get sem
    if((semid = semget(key, 0, 0)) == -1)
    {
        perror("semget error.");
        exit(1);
    }
    
    //write data
    printf("--------------------------------------\n");
    printf("                 IPCDemo              \n");
    printf("----input r to send data to server----\n");
    printf("-----------input q to leave-----------\n");
    printf("--------------------------------------\n");
    
    while(flag)
    {
        char c;
        printf("Please input command: ");
        scanf("%c", &c);
        switch (c) {
            case 'r':
            {
                printf("Data to send: ");
                sem_p(semid); // get resource
                scanf("%s", shm);
                sem_v(semid); //release res
                //clean input buffer
                while((c = getchar()) != '\n' && c != EOF);
                msg.myType = 888;
                msg.myText = 'r'; // send message to notify server read data
                msgsnd(msqid, &msg, sizeof(msg.myText), 0);
                break;
            }
            case 'q':
            {
                msg.myType = 888;
                msg.myText = 'q';
                msgsnd(msqid, &msg, sizeof(msg.myText), 0);
                flag = 0;
                break;
            }
            default:
                printf("wrong commond.\n");
                while((c = getchar()) != '\n' && c != EOF);
                break;
        }
    }
    //stop link
    shmdt(shm);
    
    return 0;
}
