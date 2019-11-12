#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

void parseInput(char *str);
int runCommand(const char *str);
void runProgram(const char *str, int haveBackgrounder, int pipefd[2], int inputOfPrev, int pipeLast);
char* trimSpaces(char* str);
#endif