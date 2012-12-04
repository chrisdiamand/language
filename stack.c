
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gc.h"
#include "stack.h"

struct stack
{
    int             len;
    int             size;
    stack_value     *st;
};

struct stack *stack_new(void)
{
    struct stack *S = GC_malloc(sizeof(struct stack));
    S->len = 0;
    S->size = 1;
    S->st = (stack_value *) GC_malloc(sizeof(stack_value) * S->size);
    return S;
}

void stack_push(struct stack *S, stack_value val)
{
    if (S->size - 1 >= S->len)
    {
        S->size += 2;
        S->st = (stack_value *) GC_realloc(S->st, sizeof(stack_value) * S->size);
    }
    S->st[S->len++] = val;
}

stack_value stack_pop(struct stack *S)
{
    if (S->len < 1)
    {
        fprintf(stderr, "Stack underflow: S = %p, len = %d, size = %d\n",
                (void *) S, S->len, S->size);
        exit(1);
    }
    return S->st[--S->len];
}

stack_value stack_get(struct stack *S, int pos)
{
    if (pos >= S->len || pos < 0)
    {
        fprintf(stderr, "Invalid stack index %d: S = %p, len = %d, size = %d\n",
                pos, (void *) S, S->len, S->size);
        exit(1);
    }
    return S->st[pos];
}

int stack_len(struct stack *S)
{
    return S->len;
}
