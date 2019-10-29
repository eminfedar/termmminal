#include "../include/PromptManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int main()
{
	setlocale(LC_ALL, "Turkish"); 
	startPrompting();

	return 0;
}