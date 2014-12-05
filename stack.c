#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

/* STACK to manage the navigation of the game different windows */
stack* stack_create() {
    int i;
	stack* retval = (stack*)malloc(sizeof(stack));
	if (retval == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}
    retval->top = 0;

    for (i=0; i<STACK_SIZE; i++) {
        retval->stack[i] = NULL;
    }

    return retval;
}
//free memory of stack
void stack_release(stack *s) {
    free(s);
}
//push to stack
void stack_push(stack *s, void* val) {
    if (s->top >= STACK_SIZE) {
        printf("stack_push#stack overflow");
        system("PAUSE");
        exit(0);
    }
    s->stack[ s->top ] = val;
    (s->top)++;
}
//pop from stack
void* stack_pop(stack *s) {
    if (s->top == 0) {
        printf("stack_pop#stack underflow");
        system("PAUSE");
        exit(0);
    }
    (s->top)--;
    return (s->stack[s->top]);
}

//find the top value of the stack without popping out
void* stack_peep(stack *s) {
    if (s->top == 0)
        return NULL;

    return (s->stack[s->top - 1]);
}
//find out if the stack is full
int stack_full(stack *s) {
    return (s->top >= STACK_SIZE);
}
