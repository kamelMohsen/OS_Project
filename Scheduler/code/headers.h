#pragma once
#include <string.h>
#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

typedef short bool;
#define stateStarted 1001
#define stateResumed 1002
#define stateStopped 1003
#define stateFinished 1004 
#define true 1
#define false 0
#define algoQueueID 1234
#define processQueueID 99
#define processMType 100
#define algoMType 200
#define SHKEY 300
#define recievingDone -1998

typedef struct algoInfo
{
    long mtype;
    int info;
} algoInfo;


typedef struct PCB 
{
    int processID;
    int runTime;
    int arrivalTime;
    int finishTime;
    int priority;
    int startTime;
    int state; 
    int waitingTime;
    bool forked;
    int TA;
    double WTA;
    pid_t forkID;
    int remainingTime;
    int lastStoppedTime;
    int memSize;
    int mStart;
    int mEnd;
} PCB;

typedef struct Alloc 
{
    int TotalSize;
    //int nAllocated;
    pid_t pid;
    bool allocated;
    int start ;
    int end;

}Alloc;

typedef struct msgPBuff
{
    long  mtype;
    struct PCB pcb;
} msgPBuff;


///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================



int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/

void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
