
#include "headers.h"
#include "queue.c"
int processMsgID;
void clearResources(int);
int readFile(PCB * pcbs );
void createMessageQueue(int *msgID, int ID);
void sendProcess(struct PCB pc, int processMsgID);
void sendAlgoNumber(int algoMsgID, int schedulingAlgo);
void schedulerDone(int);
int main(int argc, char *argv[])
{

    signal(SIGINT, clearResources);
    
    int schedulingAlgo = -1;
    int quantum = 0;
    int processesNumber = 0 ;
    int alloc = -1;
    PCB * pcbs = (PCB*)malloc(1000* sizeof(PCB)); 

    // TODO Initialization
    // 1. Read the input files.

    createMessageQueue(&processMsgID, processQueueID);


    processesNumber = readFile(pcbs);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    while(alloc<0 || alloc>1)
    {
        printf("\nWould you like to allocate memory? (1-Yes,0-No)\n");
        scanf("%d", &alloc);
    }

    while (schedulingAlgo < 1 || schedulingAlgo > 3)
    {

        printf("\nEnter the wanted scheduling algorithm:\n1.HPF\n2.SRTN\n3.RR\n");
        scanf("%d", &schedulingAlgo);
    }

    if (schedulingAlgo == 3)
    {
        while (quantum < 1)
        {
            printf("\nEnter the wanted Quantum:\n");
            scanf("%d", &quantum);
        }
    }




    // 3. Initiate and create the scheduler and clock processes

    int pid = fork();

    if (pid == -1)
    {
        perror("Error in fork!!");
    }
    else if (pid == 0)
    {
        execl("./clk.out", "clk.out", NULL);
    }

    pid = fork();

    if (pid == -1)
    {
        perror("Error in fork!!");
    }
    else if (pid == 0)
    {
        char str[64];
        sprintf(str, "%d", schedulingAlgo);
        char str1[64];
        sprintf(str1, "%d", quantum);
        char str2[64];
        sprintf(str2, "%d", processesNumber);
        char str3[64];
        sprintf(str3, "%d", alloc);
        execl("./scheduler.out", "scheduler.out", str,str1,str2,str3,NULL);
    }

    // 4. Use this function after creating the clock process to initialize clock

    
    int i=0;
    int x ;
    initClk();
    while (true)
    {
        
            x = getClk();
            if(i < processesNumber && pcbs[i].arrivalTime == x)
            {
                while(i < processesNumber && pcbs[i].arrivalTime == x )
                {
                    sendProcess(pcbs[i], processMsgID);
                    i++;

                }
                struct PCB temp;
                temp.processID = recievingDone;
                temp.remainingTime = -1;
                sendProcess(temp, processMsgID);
            }
    }

    // 7. Clear clock resources
    //raise(SIGINT);   
    destroyClk(true);
    raise(SIGKILL);
}

void clearResources(int signum)
{
    
    //TODO Clears all resources in case of interruption
     
     destroyClk(true);
     msgctl(processMsgID,IPC_RMID,NULL);
     raise(SIGKILL);

}

void sendProcess(struct PCB pcb, int processMsgID)
{
    int send_val;
    struct msgPBuff temp;

    temp.pcb = pcb;
    temp.mtype = processMType;

    send_val = msgsnd(processMsgID, &temp, sizeof(temp.pcb), !IPC_NOWAIT);

    if (send_val == -1)
    {
        perror("Error in Sending Process");
    }
}

void createMessageQueue(int *msgID, int ID)
{

    (*msgID) = msgget((ID), IPC_CREAT | 0666);

    if ((*msgID) == -1)
    {
        perror("Error in create");
    }
}

int readFile(PCB * pcbs )
{
    int i=0;
    int info[5];
    FILE *fp;
    fp = fopen("processes.txt", "r");
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (line[0] != '#')
        {
            sscanf(line, "%d\t%d\t%d\t%d\t%d", &info[0], &info[1], &info[2], &info[3],&info[4]);
            struct PCB temp;
            temp.processID = info[0];
            temp.arrivalTime = info[1];
            temp.runTime = info[2];
            temp.priority = info[3];
            temp.remainingTime = info[2];
            temp.forked=false;
            temp.lastStoppedTime =info[1];
            temp.state=0;
            temp.memSize = info[4];
            pcbs[i] = temp;
            i++;
        }
        else
            continue;
    }
    fclose(fp);
    return i;
}