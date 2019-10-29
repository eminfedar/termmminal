#include "../include/PromptManager.h"
#include "../include/CommandManager.h"
#include <stdio.h>
#include <stdlib.h>


void startPrompting()
{
    while (1)
    {
        fflush(stdout);
        printf("> ");

        char *input = NULL;
        size_t n;
        size_t inputLength = getline(&input, &n, stdin);

         // Remove the newline '\n'
        input[inputLength-1] = '\0';

        // Process the input
        parseInput(input);
        free(input);
    }
}