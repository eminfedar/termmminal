#include "../include/CommandManager.h"
#include "../include/stack.h"
#include <string.h>

void parseInput(const char *str)
{
    // struct Stack *paranthesis = createStack(20);
    // struct Stack *paranthesisIndexs = createStack(20);

    // const char symbols[5] = {';', '>', '<', '|', '&'};

    // echo hi; echo hey; echo ho

    // Remove spaces:
    /* char strNoSpace[strlen(str)];
    size_t i, newStrLength = 0;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] != ' ')
        {
            strNoSpace[newStrLength++] = str[i]
        }
    }*/
    if (str == "" || str == " ") return;

    int isSymbol = 0;
    size_t i;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] == ';')
        {
            char strLeft[i + 1];
            char strRight[(strlen(str) - (i + 1))];

            if ( str[i-1] == ' '){
                strncpy(strLeft, str, i-1);
                strLeft[i-1] = '\0';
            }
            else{
                strncpy(strLeft, str, i);
                strLeft[i] = '\0';
            }
            
            if ( str[i+1] == ' '){
                strncpy(strRight, str + (i + 2), (strlen(str) - (i+1)));
            }
            else{
                strncpy(strRight, str + (i + 1), (strlen(str) - (i)));
            }


            parseInput(strLeft);
            parseInput(strRight);
            return;
        }
    }

    printf("Bu bir komut: %s\n", str);
}

void processCommand(const char *str)
{
    if (str == "echo")
    {
    }
    else if (str == "sleep")
    {
    }
    else if (str == "quit")
    {
    }
    else
    {
    }
}