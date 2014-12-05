#ifndef _STACK_H
#define _STACK_H

#define STACK_SIZE 10

//really simple stack implementation...

typedef struct stack
{
    void* stack[STACK_SIZE];
    int top;
} stack;

stack* stack_create();
void stack_release(stack* stackp);

void stack_push(stack *s, void* val);
void* stack_pop(stack *s);
void* stack_peep(stack *s);
int stack_full(stack *s);

#endif
