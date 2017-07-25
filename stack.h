#ifndef STACK_H_
#define STACK_H_

typedef struct Stack {
	int* stack;
	int top;
	int size;
} Stack;

void initializeStack(Stack* stack_p, int size);
void push(Stack* stack_p, int element);
int pop(Stack* stack_p);
void deinitializeStack(Stack* stack_p);

#endif
