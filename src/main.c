#include "../include/PromptManager.h"
#include "../include/CommandManager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>

void exitMain()
{
	printf("\n");
	fflush(stdout);
}

int main(int argc, char **argv)
{
	signal(SIGINT, exitMain);
	setlocale(LC_ALL, "Turkish");

	mainProgramPath = argv[0];

	if (argc > 1)
	{
		int i = 0;
		for (i = 1; i < argc; i++){
			// fprintf(stderr, "argv[%d]= %s\n", i, argv[i]);
			if (strstr(argv[i], "-c") != NULL && i + 1 < argc){
				isMainSubCommand = 1;
				parseInput(trimQuotes(argv[i + 1]));
			}
		}
	}
	else
		startPrompting();

	return 0;
}