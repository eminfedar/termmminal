#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h>  /* for wait */
#include <fcntl.h>
#include <ctype.h> // isspace
#include <signal.h>
#include "../include/ThreadManager.h"
#include "../include/CommandManager.h"
#include "../include/stack.h"

// Operator checking:
char *operatorList[6] = {"&&", "||", "&", "|", ">", "<"};

// Sub Command parsing:
unsigned int subCommandCount = 0;
char *subCommands[64];

// ; splitting:
char *sentences[64];

// Piping:
int lastInputPipeFd = 0;

void parseInput(const char *str)
{
    // Return if string is empty
    if (strlen(str) == 0 || str[0] == '\0')
        return;

    // Copy the str and protect original:
    char *strProcess = malloc(strlen(str));
    strcpy(strProcess, str);

    // Find sub commands and store paranthesis indexes in a stack
    subCommandCount = 0;
    struct Stack *stack = createStack(16);
    unsigned int i;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] == '(')
            push(stack, i);
        else if (str[i] == ')')
        {
            // ORNEK: ( echo MERHABA; sleep 2 ; echo DUNYA) | ( cat | tr /A-Z/ /a-z/ )
            int strStart = pop(stack);
            int strEnd = i - strStart;

            subCommands[subCommandCount] = malloc(strEnd - 2);
            strncpy(subCommands[subCommandCount], str + strStart + 1, strEnd - 1); // -1 from left -1 from right. to remove parantheses
            subCommands[subCommandCount][strEnd - 1] = 0;

            char fullCommand[strEnd + 2];
            strncpy(fullCommand, str + strStart, strEnd + 2);
            fullCommand[strEnd + 1] = 0;

            strProcess = replace(strProcess, fullCommand, "#@SUBCMD@#");
            subCommandCount++;
        }
    }
    deleteStack(stack);
    subCommands[subCommandCount] = NULL;

    // Split string if two or more commands are existing in the string:
    runCommandFromString(strProcess);
}

void runCommandFromString(char *str)
{
    unsigned int i = 0;
    int isMultiCommand = (strstr(str, ";") != NULL);
    char *sentence = strtok(str, ";");
    while (sentence != NULL && isMultiCommand)
    {
        sentence = trimSpaces(sentence);
        sentences[i++] = sentence;    // Add it to sentence list
        sentence = strtok(NULL, ";"); // Next sentence.
    }
    sentences[i] = NULL;

    // Run command:
    if (isMultiCommand)
    {
        unsigned int c;
        for (c = 0; c < i; c++)
            runOneCommand(sentences[c]);
    }
    else
    {
        runOneCommand(trimSpaces(str));
    }
}

char *addCommand(char *command, const char *from, int commandLength)
{
    command = malloc(commandLength + 1);
    command = strncpy(command, from, commandLength);
    command[commandLength] = 0;
    command = trimSpaces(command);
    return command;
}

void runOneCommand(const char *str)
{
    char *commands[64];  // Commands stored like "echo hey","sleep 2","echo test"
    char *operators[64]; // Operators stored like "|","|","<"
    unsigned short commandCount = 0;
    unsigned short operatorCount = 0;

    unsigned int i;
    unsigned int last_i = 0;
    for (i = 0; i < strlen(str); i++)
    {
        unsigned int c;
        for (c = 0; c < 6; c++)
        {
            if (str[i] == operatorList[c][0])
            {
                if (strlen(operatorList[c]) > 1)
                {
                    if (i + 1 < strlen(str) && str[i + 1] == operatorList[c][1])
                    {
                        operators[operatorCount++] = operatorList[c];
                        commands[commandCount] = addCommand(commands[commandCount], str + last_i, i - last_i);

                        i++;
                        last_i = i + 1;
                        commandCount++;
                        break;
                    }
                }
                else
                {
                    operators[operatorCount++] = operatorList[c];
                    commands[commandCount] = addCommand(commands[commandCount], str + last_i, i - last_i);

                    last_i = i + 1;
                    commandCount++;
                    break;
                }
            }
        }
    }
    if (i != last_i)
    {
        commands[commandCount] = addCommand(commands[commandCount], str + last_i, i - last_i);

        last_i = i + 1;
        commandCount++;
    }
    operators[operatorCount] = NULL;
    commands[commandCount] = NULL;

    // Run commands:
    unsigned int k;
    for (k = 0; k < commandCount; k++)
    {
        // no operator, like "echo hey"
        if (k >= operatorCount)
            runCommand(commands[k], 0, 0);
        else
        {
            if (!strcmp(operators[k], "&&"))
            {
                // AND two process
                if (runCommand(commands[k], 0, 0) == 0)
                {
                    runCommand(commands[k + 1], 0, 0);
                }
                k++;
            }
            else if (!strcmp(operators[k], "||"))
            {
                if (runCommand(commands[k], 0, 0) != 0)
                {
                    runCommand(commands[k + 1], 0, 0);
                }
                k++;
            }
            else if (!strcmp(operators[k], "&"))
            {
                runCommand(commands[k], 1, 0);
            }
            else if (!strcmp(operators[k], "|"))
            {
                pipeToNext(commands[k]);
            }
            else if (!strcmp(operators[k], ">"))
            {
                int file;
                if ((file = open(commands[k + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666)) != NULL)
                {
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        close(1);
                        dup2(file, 1);
                        runCommand(commands[k], 0, 0);
                        close(file);
                        exit(0);
                    }
                    else if (pid > 0)
                    {
                        close(file);
                        wait(NULL);
                    }
                }
                else
                {
                    fprintf(stderr, "Dosya yazma başarısız!\n");
                }
                k++;
            }
            else if (!strcmp(operators[k], "<"))
            {
                char buffer[128];
                int file;
                size_t nread;

                if ((file = open(commands[k + 1], O_RDONLY, 0666)) != NULL)
                {

                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        int pipefd[2];
                        pipe(pipefd);

                        lastInputPipeFd = pipefd[0];

                        while ((nread = read(file, buffer, sizeof buffer)) > 0)
                            write(pipefd[1], buffer, nread);
                        
                        close(file);
                        close(pipefd[1]);
                        runCommand(commands[k], 0, 0);

                        
                        exit(0);
                    }
                    else if (pid > 0)
                    {
                        close(file);
                        wait(NULL);
                    }
                }
                else
                {
                    fprintf(stderr, "Dosya okuma başarısız!\n");
                }
                k++;
            }
            else
            {
                printf("Tanımsız operatör: '%s'\n", operators[k]);
            }
        }
    }
}

void pipeToNext(const char *process)
{
    int pipefd[2];
    pipe(pipefd);

    runCommand(process, 0, pipefd);

    lastInputPipeFd = pipefd[0];
}

int runCommand(const char *str, int haveBackgrounder, int pipefd[2])
{
    // These function for shell's inside commands.
    // For now, it only consists quit command.
    // 'cd' like commands can be added.

    if (strcmp(str, "quit") == 0 || strcmp(str, "exit") == 0)
        exit(0);

    return runProgram(str, haveBackgrounder, pipefd);
}

void exitSubProcess()
{
    fflush(stdout);
    exit(EXIT_FAILURE);
}

int runProgram(const char *str, int haveBackgrounder, int pipefd[2])
{
    char strProcess[strlen(str)];
    strcpy(strProcess, str);

    char *parameterArray[40] = {0};
    char *oneParameter = strtok(strProcess, " ");

    int isSubCommand = strstr(strProcess, "#@SUBCMD@#") != NULL;

    if (isSubCommand)
    {

        parameterArray[0] = mainProgramPath;
        parameterArray[1] = "-c";

        // Command:
        unsigned int subCmdIndex = 0;
        char *subCmd;
        while (subCommands[subCmdIndex] == NULL && subCmdIndex + 1 < subCommandCount)
            subCmdIndex++;

        subCmd = trimSpaces(subCommands[subCmdIndex]);
        subCommands[subCmdIndex++] = NULL;

        unsigned int cmdLength = strlen(subCmd);
        char *cmdWithQuotes = malloc(cmdLength + 3);
        cmdWithQuotes[0] = '"';
        strncpy(cmdWithQuotes + 1, subCmd, cmdLength);
        cmdWithQuotes[cmdLength + 1] = '"';
        cmdWithQuotes[cmdLength + 2] = 0;

        parameterArray[2] = cmdWithQuotes;
        parameterArray[3] = NULL;

    }
    else
    {
        unsigned int i = 0;
        while (oneParameter != NULL)
        {
            parameterArray[i++] = oneParameter;
            oneParameter = strtok(NULL, " ");
        }
        parameterArray[i] = NULL;
    }

    /*Spawn a child to run the program.*/
    pid_t pid = fork();
    if (pid == 0) // if chlid
    {
        signal(SIGINT, exitSubProcess);

        // Read from pipe if available
        if (lastInputPipeFd)
        {
            dup2(lastInputPipeFd, 0); // input from previous pipe's output
            lastInputPipeFd = 0;
        }

        // Write to pipe if pipe given
        if (pipefd)
        {
            dup2(pipefd[1], 1); // output to pipe if not the last program;

            close(pipefd[1]);
            close(pipefd[0]);
        }

        if (execvp(parameterArray[0], parameterArray) < 0)
        {
            fprintf(stderr, "Böyle bir komut veya program yok: %s\n", parameterArray[0]);
            exit(EXIT_FAILURE);
        }

        return 0;
    }
    else if (pid > 0)
    {
        if (pipefd)
            close(pipefd[1]);

        if (haveBackgrounder)
        {
            waitProcessInBackground(pid, 0);
            return 0;
        }
        else
        {
            int processReturnValue;
            waitpid(pid, &processReturnValue, 0);
            // fprintf(stderr, "process bitti: %d\n", pid);
            return processReturnValue;
        }
    }
}

// Useful string operations:
char *replace(char *src, char *search, char *replace)
{
    char *found = strstr(src, search);
    if (found != NULL)
    {
        char *newString = malloc(strlen(src) - strlen(search) + strlen(replace) + 1);
        strncpy(newString, src, found - src);
        strcpy(newString + (int)(found - src), replace);
        strcpy(newString + (int)(found - src) + strlen(replace), found + strlen(search));
        newString[strlen(src) - strlen(search) + strlen(replace)] = 0;
        return newString;
    }

    return NULL;
}
char *trimSpaces(char *str)
{
    /* Trim Spaces */
    unsigned int len = strlen(str);
    while (isspace(str[len - 1]))
        str[--len] = 0;
    while (*str && isspace(*str))
        ++str, --len;
    return str;
}
char *trimQuotes(char *str)
{
    /* Trim Spaces */
    unsigned int len = strlen(str);
    while (str[len - 1] == '"')
        str[--len] = 0;
    while (*str && *str == '"')
        ++str, --len;
    return str;
}
char *stringCopyN(char *dest, char *src, unsigned int length)
{
    unsigned int i;
    for (i = 0; i < length; i++)
    {
        dest[i] = *(src + i);
    }
    return dest;
}