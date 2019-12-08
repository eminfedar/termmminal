#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H
#include <stddef.h>

// Global variables
char *mainProgramPath;
int isMainSubCommand;

void parseInput(const char *str);
void runCommandFromString(char *str);

// Add 1 command to commands[64]
char* addCommand(char *to, const char* from, int charN);

// Run functions:
void runOneCommand(const char *str);
int runCommand(const char *str, int haveBackgrounder, int pipefd[2]);
int runProgram(const char *str, int haveBackgrounder, int pipefd[2]);

// Operations:
void pipeToNext(const char *process);

// String operations
char *trimSpaces(char *str);
char *trimQuotes(char *str);
char *replace(char *src, char *search, char *replace);
char *stringCopyN(char *dest, char *src, unsigned int length);
#endif