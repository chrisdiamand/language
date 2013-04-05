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
};

#endif
