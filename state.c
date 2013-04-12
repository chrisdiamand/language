#include <stdio.h>

#include "class.h"
#include "dict.h"
#include "gc.h"
#include "stack.h"
#include "state.h"

/* Put memb into the current scope in S */
void state_class_to_scope(struct state *S, char *name, struct class_t *cl)
{
    struct dict *scope = (struct dict *) stack_top(S->typescope);
    /* FIXME: Check it doesn't already exist */
    dict_set(scope, name, cl);
}

struct class_t *state_find_class(struct state *S, char *name)
{
    int i;
    for (i = stack_len(S->typescope) - 1; i >= 0; i--)
    {
        struct dict *scope = stack_get(S->typescope, i);
        struct class_t *cl = (struct class_t *) dict_get(scope, name);
        if (cl)
            return cl;
    }
    return NULL;
}

struct state *state_new(void)
{
    struct state *S = GC_malloc(sizeof(struct state));

    S->typescope = stack_new();
    stack_push(S->typescope, dict_new());

    S->class_object = class_new(NULL);
    state_class_to_scope(S, "Object", S->class_object);
    S->class_class = class_new(S->class_object);
    S->class_method = class_new(S->class_object);

    return S;
}
