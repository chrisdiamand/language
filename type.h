#ifndef TYPE_H
#define TYPE_H

#include "class.h"
#include "state.h"

struct state;

struct type_t
{
    struct class_t      *cl;
    struct type_t       **params;
    union
    {
        struct class_t      *cl;
        struct method_t     *method;
    } v;
};

struct type_t *type_new(struct class_t *);
struct type_t *type_from_class(struct state *, struct class_t *);
struct type_t *type_from_method(struct state *, struct method_t *);

#endif /* TYPE_H */
