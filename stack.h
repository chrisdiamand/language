
#ifndef STACK_H
#define STACK_H

typedef void *stack_value;

struct stack;

struct stack *stack_new(void);
void stack_push(struct stack *, stack_value);
stack_value stack_pop(struct stack *);
stack_value stack_top(struct stack *);
stack_value stack_get(struct stack *, int);
int stack_len(struct stack *);

#endif

