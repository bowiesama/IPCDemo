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

//delete sem
int del_sem(int sem_id)
{
    union semunion tmp;
    if(semctl(sem_id, 0, IPC_RMID, tmp) == -1)
    {
        perror("Delete Semaphore Error.");
        return -1;
    }
    return 0;
}

int create_sem(key_t key)
{
    int sem_id;
    if((sem_id = semget(key, 1, IPC_CREAT|0666)) == -1)
    {
        perror("Semget error.");
        exit(-1);
    }
    init_sem(sem_id, 1);// set 1 when init,means resource is not occupied.
    return sem_id;
}

int main()
{
    key_t key;
    int  shmid, semid, msqid;
    char *shm;
    char data[] = "this is server.";
    struct shmid_ds shm_buffer;  // to delete share memory
    struct msqid_ds msq_buffer;  // to delete msg queue
    struct msg_form msg; // to notify share memory has update
    
    //get key
    if(key = ftok("/tmp/README", 'z') < 0)
    {
        perror("ftok error.");
        exit(1);
    }
    
    //create share memory
    if((shmid = shmget(key, 1024, IPC_CREAT|0666)) == -1)
    {
        perror("Create Shared Memory Error");
        exit(1);
    }
    
    //link memory
    shm = (char*)shmat(shmid, 0, 0);
    
    //create msg queue
    if((msqid = msgget(key, IPC_CREAT|0777)) == -1)
    {
        perror("MSGGET ERROR");
        exit(1);
    }
    
    //create sem
    semid = create_sem(key);
    
    //read data
    while(1)
    {
        msgrcv(msqid, &msg, 1, 888, 0);
        if(msg.myText == 'q') //quit
            break;
        if(msg.myText == 'r') //read memory
        {
            sem_p(semid);
            printf("%s\n", shm);
            sem_v(semid);
        }
    }
    
    //stop link
    shmdt(shm);
    
    //delete memory msqueue sem
    shmctl(shmid, IPC_RMID, &shm_buffer);
    msgctl(msqid, IPC_RMID, &msq_buffer);
    del_sem(semid);
    return 0;
    
    //
}
