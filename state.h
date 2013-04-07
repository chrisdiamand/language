#ifndef STATE_H
#define STATE_H

struct state
{
    /* Base classes */
    struct class_t          *class_object;
    struct class_t          *class_class;
    struct class_t          *class_method;
    /* Primitives */
    struct class_t          *class_int;
    struct class_t          *class_double;

    /* Stack of dictionaries of classes to know which classes
     * are visible in the current scope */
    struct stack            *typescope;
};

struct state *state_new(void);
void state_class_to_scope(struct state *, char *, struct class_t *);

#endif
