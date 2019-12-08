// TAKEN FROM geeksforgeeks.org
#include <stdlib.h> 
#include <limits.h> 

struct Stack { 
	int top; 
	int capacity; 
	int* array; 
}; 

struct Stack* createStack(int capacity) 
{ 
	struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack)); 
	stack->capacity = capacity; 
	stack->top = -1; 
	stack->array = (int*)malloc(stack->capacity * sizeof(int)); 
	return stack; 
} 
 
int isFull(struct Stack* stack) 
{ 
	return stack->top == (int)(stack->capacity - 1); 
} 

int isEmpty(struct Stack* stack) 
{ 
	return stack->top == -1; 
} 

void push(struct Stack* stack, int item) 
{ 
	if (isFull(stack)) 
		return; 
	stack->array[++stack->top] = item; 
} 

int pop(struct Stack* stack) 
{ 
	if (isEmpty(stack)) 
		return INT_MIN; 
	return stack->array[stack->top--]; 
} 

int peek(struct Stack* stack) 
{ 
	if (isEmpty(stack)) 
		return INT_MIN; 
	return stack->array[stack->top]; 
}

void deleteStack(struct Stack* stack)
{
	free(stack->array);
	free(stack);
}