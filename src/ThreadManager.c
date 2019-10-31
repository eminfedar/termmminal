#include <pthread.h>
#include <stdio.h>
#include <sys/wait.h>  /* for wait */

void _waitFunc(int pid)
{
    int status;
    waitpid(pid, &status, 0);
    printf("\n[#%d Process Returned]: %d\n> ", pid, status);
    fflush(stdout);
}

void waitProcessInBackground(int pid)
{
    pthread_t tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, (void *)&_waitFunc, pid);
}