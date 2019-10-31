#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

void parseInput(const char *str);
void processCommand(const char *str, int waitProcess);
void runProgram(const char *cmd, const char *params, int waitProcess);
#endif