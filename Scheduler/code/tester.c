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

typedef struct algoInfo
{
    long mtype;
    int info;
} algoInfo;

int main() {

	algoInfo mymsg;
	int algoqueue = msgget(99, IPC_CREAT | 0644);
	int message = msgrcv(algoqueue, &mymsg, sizeof(mymsg.info), 2, IPC_NOWAIT);
	printf("The Algo number is %d \n", mymsg.info);
	return 0;
}
