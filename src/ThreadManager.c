#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> /* for wait */

void _waitFunc(void *_pid)
{
    int pid = *((int *)_pid);
    int status;
    waitpid(pid, &status, 0);
    printf("\n[#%d Process Returned]: %d\n> ", pid, status);
    fflush(stdout);
    free(_pid);
}
void _waitFuncSilent(void *_pid)
{
    int pid = *((int *)_pid);
    int status;
    waitpid(pid, &status, 0);
    fflush(stdout);
    free(_pid);
}

void waitProcessInBackground(int pid, int silent)
{
    pthread_t tid;
    pthread_attr_t attr;

    int *pidAlloc = malloc(sizeof(pid));
    *pidAlloc = pid;
    pthread_attr_init(&attr);

    if(silent)
        pthread_create(&tid, &attr, (void *)&_waitFuncSilent, (void *)pidAlloc);
    else
        pthread_create(&tid, &attr, (void *)&_waitFunc, (void *)pidAlloc);
}