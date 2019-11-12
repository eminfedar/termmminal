
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h>  /* for wait */
#include <ctype.h> // isspace
#include <signal.h> 
#include "../include/ThreadManager.h"
#include "../include/CommandManager.h"
// #include "../include/stack.h"

void parseInput(char *str)
{
    // Return if string is empty
    if (strlen(str) == 0 || str[0] == '\0')
        return;

    // Copy the str and protect original:
    char strProcess[strlen(str)];
    strcpy(strProcess, str);

    // Split string if two or more commands are existing in the string:
    size_t i = 0;
    char *sentence = strtok(str, ";");
    int isMultiCommand = (strstr(sentence, ";") != NULL);
    char *sentences[64];
    while (sentence != NULL && isMultiCommand)
    {
        sentence = trimSpaces(sentence);
        sentences[i++] = sentence; // Add it to sentence list
        sentence = strtok(NULL, ";"); // Next sentence.
    }
    if (isMultiCommand)
    {
        sentences[i] = NULL;

        size_t c;
        for (c = 0; c < i; c++)
            parseInput(sentences[c]);

        return;
    }

    // Piping: ( echo hey | cat )
    const char *pipeWord = strstr(strProcess, "|");
    int havePipe = (pipeWord != NULL && pipeWord[1] != '|') ? 1 : 0;

    // Backgrounding:
    const char *backgroundWord = strstr(strProcess, "&");
    int haveBackgrounder = (backgroundWord != NULL && backgroundWord[1] != '&') ? 1 : 0;
    if (haveBackgrounder)
        strtok(strProcess, "&"); // remove the last & from string.

    if (havePipe)
    {
        // Copy the str and protect original:
        char strPipeSearch[strlen(strProcess)];
        strcpy(strPipeSearch, strProcess);

        // Turn parameters into an array:
        char *pipedProcesses[40] = {NULL};
        char *oneProcess = strtok(strPipeSearch, "|");
        int i = 0;
        while (oneProcess != NULL)
        {
            pipedProcesses[i] = trimSpaces(oneProcess);
            i++;
            oneProcess = strtok(NULL, "|");
        }
        pipedProcesses[i] = NULL;

        int pipefd[2];
        int inputOfPrev = 0;

        int a;
        for (a = 0; a < i; a++)
        {
            pipe(pipefd);
            runProgram(pipedProcesses[a], 0, pipefd, inputOfPrev, a == i - 1 ? 1 : 0);

            inputOfPrev = pipefd[0];
        }
    }
    else
    {
        // Run normally
        if (!runCommand(strProcess))
        {
            runProgram(strProcess, haveBackgrounder, 0, 0, 0);
        }
    }
}

int runCommand(const char *str)
{
    char cmd[256], params[0xFFFF];
    sscanf(str, "%s %65535[^\n]", cmd, params);

    if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0)
    {
        exit(0);
    }

    return 0;
}

void exitSubProcess()
{
    fflush(stdout);
    exit(EXIT_FAILURE);
}

void runProgram(const char *str, int haveBackgrounder, int pipefd[2], int inputOfPrev, int pipeLast)
{
    char strProcess[strlen(str)];
    strcpy(strProcess, str);

    char *parameterArray[40] = {0};
    char *oneParameter = strtok(strProcess, " ");

    size_t i = 0;
    while (oneParameter != NULL)
    {
        parameterArray[i++] = oneParameter;
        oneParameter = strtok(NULL, " ");
    }
    parameterArray[i] = NULL;

    /*Spawn a child to run the program.*/
    pid_t pid = fork();
    if (pid == 0) // if chlid
    {
        signal(SIGINT, exitSubProcess);
        if (pipefd)
        {
            dup2(inputOfPrev, 0); // input from previous pipe's output

            if (!pipeLast)
            {
                dup2(pipefd[1], 1); // output to pipe if not the last program;
            }
            close(pipefd[0]);
            close(pipefd[1]);
        }

        if (execvp(parameterArray[0], parameterArray) < 0)
        {
            fprintf(stderr, "Böyle bir komut veya program yok: %s\n", parameterArray[0]);
            exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
    {
        if(pipefd)
            close(pipefd[1]);
        
        if (haveBackgrounder)
            waitProcessInBackground(pid);
        else
            waitpid(pid, NULL, 0);
    }
}

char *trimSpaces(char *str)
{
    /* Trim Spaces */
    size_t len = strlen(str);
    while (isspace(str[len - 1]))
        str[--len] = 0;
    while (*str && isspace(*str))
        ++str, --len;
    return str;
}