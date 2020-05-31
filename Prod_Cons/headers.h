#pragma once
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/msg.h>
#include <string.h>



#define true 1
#define false 0
#define sharedMemory 9999
#define mutexKey 1111
#define emptyKey 2222
#define fullKey 3333
#define userVal 4444
#define prod 5555
#define cons 9999
#define userValType 6666
#define bufinfoType 7777
#define prodrcv 3100
#define count 1998


typedef struct msgbuff
{
    long mtype;
    int userValue;
}msgbuff;

