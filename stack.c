#include <stdlib.h>
#include <stdio.h>

#include "stack.h"



void initializeStack(Stack* stack_p, int size) {
	stack_p -> stack = (int*)(malloc(sizeof(int) * size));
	stack_p -> top = -1;
	stack_p -> size = size;
}
void push(Stack* stack_p, int element) {
	if(stack_p -> top == (stack_p -> size - 1)) {
		perror("Stack overflow");
	} else {
		stack_p -> stack[stack_p -> top + 1] = element;
		stack_p -> top = stack_p -> top + 1;
	}
}
int pop(Stack* stack_p) {
	int element;
	if(stack_p -> top == -1) {
		perror("Popping empty stack");
		return -1;
	} else {
		element = stack_p -> stack[stack_p -> top];
		stack_p -> top = stack_p -> top - 1;
		return element;
	}
}
void deintializeStack(Stack* stack_p) {
	free(stack_p -> stack);
	stack_p -> size = 0;
}

