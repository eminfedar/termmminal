#include "../include/CommandManager.h"
#include "../include/stack.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>    /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h>  /* for wait */

void parseInput(const char *str)
{
    // Return if string is empty
    if (strlen(str) == 0 || str[0] == '\0')
        return;

    // Copy the str and protect original:
    const char strProcess[strlen(str)];
    strcpy(strProcess, str);

    // Split string if two or more commands are existing in the string:
    size_t i = 0;
    char *sentence = strtok(str, ";");
    int isMultiCommand = (strcmp(sentence, strProcess) != 0);
    char *sentences[64];
    while (sentence != NULL && isMultiCommand)
    {
        /* Trim Spaces */
        size_t len = strlen(sentence);
        while (isspace(sentence[len - 1]))
            sentence[--len] = 0;
        while (*sentence && isspace(*sentence))
            ++sentence, --len;

        sentences[i++] = sentence; // Add it to sentence list

        sentence = strtok(NULL, ";"); // Next sentence.
    }
    if (isMultiCommand)
    {
        sentences[i] = NULL;

        size_t c;
        for (c = 0; c < i; c++)
        {
            parseInput(sentences[c]);
        }
        return;
    }

    // Returns NULL if didn't find the splitter token.
    /* char *or = strtok(str, "||");
    char *and = strtok(str, "&&");
    char *outputForward = strtok(str, ">");
    char *inputForward = strtok(str, "<");
    char *pipe = strtok(str, "|");
    char *backgrounder = strtok(str, "&");
    */

    processCommand(strProcess);
}

void processCommand(const char *str)
{
    char cmd[256], params[0xFFFF];
    sscanf(str, "%s %65535[^\n]", cmd, params);

    if (strcmp(cmd, "echo") == 0)
    {
        printf("%s\n", params);
    }
    else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0)
    {
        exit(0);
    }
    else
    {
        runProgram(cmd, params);
    }
}

void runProgram(const char *cmd, const char* params)
{
    // Turn parameters into an array:
    char *paramArr[30] = {cmd, NULL};
    char *aParam = strtok(params, " ");
    size_t i = 1;
    while (aParam != NULL)
    {
        paramArr[i] = aParam;
        i++;
        aParam = strtok(NULL, " ");
    }
    paramArr[i] = "\0";

    /*Spawn a child to run the program.*/
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execvp(paramArr[0], paramArr) < 0)
        {
            fprintf(stderr, "Böyle bir komut veya program yok: %s\n", cmd);
        }
    }
    else
    {
        waitpid(pid, 0, 0); /* wait for child to exit */
    }
}