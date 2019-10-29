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

    // Split string if two or more commands are existing in the string:
    size_t i;
    char *sentence = strtok(str, ";");
    int isMultiCommand = (sentence != str);
    while (sentence != NULL && sentence != str)
    {
        isMultiCommand = 1;

        /* Trim Spaces */
        size_t len = strlen(sentence);
        while (isspace(sentence[len - 1]))
            sentence[--len] = 0;
        while (*sentence && isspace(*sentence))
            ++sentence, --len;

        parseInput(sentence);

        sentence = strtok(NULL, ";"); // Next sentence.
    }
    if (isMultiCommand)
        return;

    // Returns NULL if didn't find the splitter token.
    char *or = strtok(str, "||");
    char *and = strtok(str, "&&");
    char *outputForward = strtok(str, ">");
    char *inputForward = strtok(str, "<");
    char *pipe = strtok(str, "|");
    char *backgrounder = strtok(str, "&");

    processCommand(str);
}

void processCommand(const char *str)
{
    char cmd[256], param1[0xFFFF];
    sscanf(str, "%s %s", cmd, param1);

    if (strcmp(cmd, "echo") == 0)
    {
        printf("%s\n", param1);
    }
    else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0)
    {
        exit(0);
    }
    else
    {
        runProgram(str);
    }
}

void runProgram(const char *str)
{
    char cmd[256], params[256];
    sscanf(str, "%s %s", cmd, params);

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
    paramArr[i] = NULL;

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