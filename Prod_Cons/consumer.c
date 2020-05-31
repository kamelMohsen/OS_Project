#include "headers.h"


int shmid;
int producerQueue;
int consumerQueue;
struct msgbuff mymessage;
int *buffer;
int mutexSem;
int countmid;
int * numOfElements;
int userSize;


void intiateSharedMemory();
void intiateSem();
void recieveSize();
void consumer();
void wakeupReceiver();
void wakeupSender();
void intiateQueues();



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
    intiateQueues(); //initializes queues
    recieveSize(); //receives size from the producer on the producer queues.
    intiateSharedMemory();  
    intiateSem();
    
    //calling the consumer
    consumer();

    return 0;
}



void consumer()
{
    do
    {
        down(mutexSem);
        printf("I downed the size is : %d\n",numOfElements[0]);
        if(numOfElements[0] == 0) //if the buffer is empty
        {
            up(mutexSem);
            wakeupReceiver(); //block and listen on the producer Queue for a wake up message
        }
        else if( numOfElements[0] < (userSize/sizeof(int)) ) //if the number of elements is less than the max size of the buffer, consume normally
        {
            int item = buffer[numOfElements[0] - 1];
            numOfElements[0]--;
            printf("Consumed %d at locatino %d \n", item, numOfElements[0]);
            up(mutexSem);
        }
        else if( numOfElements[0] == (userSize/sizeof(int)) ) //the buffer is full
        {
            int item = buffer[numOfElements[0] - 1];
            numOfElements[0]--;
            printf("Consumed %d at locatino %d \n", item, numOfElements[0]);
            up(mutexSem);
            wakeupSender(); //send a wakeup message to the producer
        }
        
    }while(true);
}



void intiateSem()
{
    //Create mutex semaphores
    mutexSem = semget(mutexKey, 1, 0666 | IPC_CREAT);
    
    if (mutexSem == -1 )
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



void recieveSize()
{
    mymessage.userValue = 0;
    int size = msgrcv(producerQueue,&mymessage,sizeof(mymessage.userValue),userValType,!IPC_NOWAIT);
    if(size == -1)
    {
        perror("Failed to recieve size from producer due to ");
    }

    userSize = mymessage.userValue;
}



void intiateSharedMemory()
{
    //creates a shared memory for the buffer
    shmid = shmget(sharedMemory, userSize, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("CBuffer Shared Memory failed to allocate due to:");
        exit(1);
    }
    else
    {
        printf("CBuffer Shared Memory allocated successfully. \n");
    }

    buffer = (int *)shmat(shmid, 0, 0);
    if (buffer == (int *)-1)
    {
        perror("CBuffer failed to attach");
    }
    
    //This initiates shared memory that holds the number of elements
    countmid = shmget(count, 1, IPC_CREAT | 0666);
    if (countmid < 0)
    {
        perror("CCount Shared Memory failed to allocate due to:");
        exit(1);
    }
    else
    {
        printf("CCount Shared Memory allocated successfully. \n");
    }

    numOfElements = (int *)shmat(countmid, 0, 0);
    if (numOfElements == (int *)-1)
    {
        perror("CCount failed to attach");
    }
  

}

void wakeupReceiver()
{
    write(1, "The Consumer is now sleeping, waiting for wakeup from producer. \n",66);
    msgbuff result;
    int MSGRCV = msgrcv(producerQueue, &result, sizeof(result.userValue), prodrcv, !IPC_NOWAIT);

}


void wakeupSender()
{
    msgbuff result;
    result.mtype = prodrcv;
    result.userValue = 2937;
    int MSGSND = msgsnd(consumerQueue, &result, sizeof(result.userValue), IPC_NOWAIT);

}



void intiateQueues()
{
     //int userValQueue = msgget(userVal, IPC_CREAT | 0666); //userVal contains the user size that the producer will send to the consumer
    producerQueue = msgget(prod, IPC_CREAT | 0666);       //produceQueue contains the messages that the producer will send to wake up the consumer
    consumerQueue = msgget(cons, IPC_CREAT | 0666);       //consumerQueue contains the messages that the consumer will send to wake up the producer
}


