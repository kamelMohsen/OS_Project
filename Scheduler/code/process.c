#include "headers.h"
#include <time.h>

/* Modify this file as needed*/
clock_t remainingtime;

int main(int agrc, char * argv[])
{


    clock_t current_time,start_t,last;
    current_time = 0;
    remainingtime=(clock_t)atoi(argv[1]);
    last=0;

    while (remainingtime > current_time)
    {  
        start_t = clock();
        current_time = (double)((start_t)/(CLOCKS_PER_SEC)) ;
        // if(last!=current_time)
        // {
        //     last=current_time;
        // }    
    }
    kill(getppid(),SIGUSR1);
    exit(0);
}
