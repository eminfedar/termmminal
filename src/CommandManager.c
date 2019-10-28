#include "../include/CommandManager.h"
#include "../include/stack.h"
#include <string.h>

void parseInput(const char *str)
{
    // Return if string is empty
    if (strlen(str) == 0 || str[0] == '\0')
        return;

    // Split string if two or more commands are existing in the string:
    size_t i;
    for (i = 0; i < strlen(str); i++)
    {
        // SPLITTER (;)
        // ; splits commands and just make them run seperately.
        if (str[i] == ';')
        {
            char strLeft[i + 1];
            char strRight[(strlen(str) - (i + 1))];

            // Left part:
            if (i > 0)
            {
                if (str[i - 1] == ' ')
                {
                    strncpy(strLeft, str, i - 1);
                    strLeft[i - 1] = '\0';
                }
                else
                {
                    strncpy(strLeft, str, i);
                    strLeft[i] = '\0';
                }
                parseInput(strLeft);
            }

            // Right part:
            if ((strlen(str) - (i)) > 0)
            {
                if (str[i + 1] == ' ')
                {
                    strncpy(strRight, str + (i + 2), (strlen(str) - (i + 1)));
                }
                else
                {
                    strncpy(strRight, str + (i + 1), (strlen(str) - (i)));
                }
                parseInput(strRight);
            }
            return;
        }
        else if (str[i] == "|")
        {
            // It can be PIPE "|" or OR "||"
            return;
        }
        else if (str[i] == "&")
        {
            // It can be Background Starter("&") or AND("&&")
            return;
        }
    }
    processCommand(str);
}

void processCommand(const char *str)
{
    if (strcmp(str, "echo") == 0)
    {
    }
    else if (strcmp(str, "sleep") == 0)
    {
    }
    else if (strcmp(str, "quit") == 0 || strcmp(str, "exit") == 0)
    {
        exit(0);
    }
    else
    {
        printf("Tanimsiz komut: %s\n", str);
    }
}