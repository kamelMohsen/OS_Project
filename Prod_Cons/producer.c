#include "headers.h"


int shmid;         //shared memory size
int userSize;      //user size for the buffer
int producerQueue; //the queue the producer sends on
int consumerQueue; //the queue the consumer sends on
int *buffer;
int mutexSem;
struct msgbuff mymessage;
int countmid;
int * numOfElements;

void handler(int);

int itemGenerator(); //creates random numbers using time as a seed
void takeInput();    //takes size of buffer from the user
void intiateSharedMemory(); //initiates two memory segments; one for the buffer, and one containing the number of elements
void intiateSem(); //initiates the mutex semaphore
void sendSize(); //sends the size of the buffer to the consumer in a queue in order to perform necessary checks
void producer(); 
void wakeupReceiver(); //listens for a wakeup a call from the consumer on the consumer queue
void intiateQueues();  //initialises one queue for the producer and one queue for the consumer 
void wakeupSender();   //sends a wakeup call to the consumer on the producer queue


union Semun 
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    unsigned short *array; /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

union Semun semun;

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}



int main()
{
    signal(SIGINT, handler);
    signal(SIGALRM,SIG_IGN);

    intiateQueues(); 
    takeInput(); //user input for size of buffer
    sendSize(); //sends size to the consumer
    intiateSem(); //starts the mutex semaphore
    intiateSharedMemory();
    
    //start the producer
    producer();

    

    return 0;
}


void producer()
{
    do
    {
    
        down(mutexSem);
        printf("I downed the size is : %d\n",numOfElements[0]);
        if ( numOfElements[0] >= (userSize/sizeof(int)) ) //if number of elements in the buffer is equal to the size of buffer
        {
            up(mutexSem);
            wakeupReceiver(); //wait for the consumer to wake me up
        }
        else if( numOfElements[0] < (userSize/sizeof(int)) && numOfElements[0] > 0) //if the number of elements in the buffer is less than the size of the buffer but greater than zero
        {
            int item = itemGenerator();
            buffer[numOfElements[0]] = item;
            numOfElements[0]++;
            printf("Produced %d at location %d \n", item, numOfElements[0] -1);
            up(mutexSem);
        }
        else if (numOfElements[0] == 0) //if the buffer is empty
        {
            int item = itemGenerator();
  
            buffer[numOfElements[0]] = item;
            numOfElements[0]++;
            printf("Produced %d at location %d \n", item, numOfElements[0] -1);
            wakeupSender();
            up(mutexSem);
        }
        sleep(1);
        
    }while(true);
}




void sendSize()
{
    //sends a message to the consumer including the buffer size
    mymessage.mtype = userValType;
    mymessage.userValue = userSize;
    int MSGSND = msgsnd(producerQueue, &mymessage, sizeof(mymessage.userValue), IPC_NOWAIT);

}

void intiateSem()
{
    //Create mutex semaphores
    mutexSem = semget(mutexKey, 1, 0666 | IPC_CREAT);
    
    if (mutexSem == -1 )//|| emptySem == -1 || fullSem == -1)
    {
        perror("Error in creating Semaphore");
        exit(-1);
    }

    //initialise the mutex semaphore with 1
    semun.val = 1;
    if (semctl(mutexSem, 0, SETVAL, semun) == -1)
    {
        perror("Error in mutex semctl");
    }
}


void intiateSharedMemory()
{
    //creates a shared memory segment with the user input size + 2 for array size and Last available index
  
    shmid = shmget(sharedMemory, userSize, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("PBuffer Shared Memory failed to allocate due to:");
        exit(1);
    }
    else
    {
        printf("PBuffer Shared Memory allocated successfully. \n");
    }

    buffer = (int *)shmat(shmid, 0, 0);
    if (buffer == (int *)-1)
    {
        perror("PBuffer failed to attach");
    }
    
    //This initiates shared memory that holds the number of elements
    countmid = shmget(count, sizeof(int), IPC_CREAT | 0666);
    if (countmid < 0)
    {
        perror("PCount Shared Memory failed to allocate due to:");
        exit(1);
    }
    else
    {
        printf("PCount Shared Memory allocated successfully. \n");
    }

    numOfElements = (int *)shmat(countmid, 0, 0);
    if (numOfElements == (int *)-1)
    {
        perror("PCount failed to attach");
    }
    numOfElements[0] = 0;

}

void takeInput()
{

    //User inputs the size of the buffer
    printf("Enter the size of the buffer you need: \n");
    scanf("%d", &userSize);
    userSize = userSize * sizeof(int);
}




void handler(int signum)
{
    msgctl(producerQueue,IPC_RMID,NULL);
    msgctl(consumerQueue,IPC_RMID,NULL);
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(countmid, IPC_RMID, NULL);
    killpg(getpgrp(), SIGINT);
    int result = semctl(mutexSem, IPC_RMID, 0);
    exit(2);
}

int itemGenerator()
{
    srand(time(0));
    return (rand() % 1000);
}

void  wakeupReceiver()
{
    write(1, "The producer is now sleeping, waiting for wakeup from consumer. \n",66);
    msgbuff result;
    int MSGRCV = msgrcv(consumerQueue, &result, sizeof(result.userValue), prodrcv, !IPC_NOWAIT);
  
}

void wakeupSender()
{
    
    msgbuff result;
    result.mtype = prodrcv;
    result.userValue = 23;
    int MSGSND = msgsnd(producerQueue, &result, sizeof(result.userValue), IPC_NOWAIT);

}

void intiateQueues()
{
     //int userValQueue = msgget(userVal, IPC_CREAT | 0666); //userVal contains the user size that the producer will send to the consumer
    producerQueue = msgget(prod, IPC_CREAT | 0666);       //produceQueue contains the messages that the producer will send to wake up the consumer
    consumerQueue = msgget(cons, IPC_CREAT | 0666);       //consumerQueue contains the messages that the consumer will send to wake up the producer
}



