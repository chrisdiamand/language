#ifndef STATE_H
#define STATE_H

#include "type.h"

struct state
{
    /* Base classes */
    struct class_t          *class_object;
    struct class_t          *class_class;
    struct class_t          *class_method;
    /* Primitives */
    struct class_t          *class_int;
    struct class_t          *class_double;

    /* Stack of classes to know which classes are visible
     * in the current scope during the first pass */
    struct stack            *typescope;
};

struct state *state_new(void);
struct type_t *state_lookup_global(struct state *, char *);

#endif
