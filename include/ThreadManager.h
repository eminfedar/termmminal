#ifndef THREADMANAGER_H
#define THREADMANAGER_H

void _waitFunc(void* pid);
void _waitFuncSilent(void* pid);
void waitProcessInBackground(int pid, int silent);

#endif