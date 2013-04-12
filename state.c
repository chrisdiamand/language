#include <assert.h>
#include <stdio.h>

#include "class.h"
#include "dict.h"
#include "gc.h"
#include "stack.h"
#include "state.h"

// Look up a global (usually a class name) from the stack of classes
struct type_t *state_lookup_global(struct state *S, char *name)
{
    int i;
    for (i = stack_len(S->typescope) - 1; i >= 0; i--)
    {
        struct class_t *cl = stack_get(S->typescope, i);
        struct type_t *t;
        assert(cl);
        t = class_get_static_member(cl, name);
        if (t)
            return t;
    }
    return NULL; // Can't find it
}

struct state *state_new(void)
{
    struct state *S = GC_malloc(sizeof(struct state));

    S->typescope = stack_new();

    S->class_object = class_new("Object", NULL);
    S->class_class = class_new("Class", S->class_object);
    S->class_method = class_new("Method", S->class_object);

    return S;
}
