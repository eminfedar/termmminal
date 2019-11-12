#include "../include/PromptManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>

void exitMain(){
	printf("\n> ");
	fflush(stdout);
}

int main()
{
	signal(SIGINT, exitMain);

	setlocale(LC_ALL, "Turkish"); 
	startPrompting();

	return 0;
}