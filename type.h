#ifndef TYPE_H
#define TYPE_H

#include "class.h"

struct type_t
{
    struct class_t  *cl;
    struct type_t   **params;
};

struct type_t *type_new(struct class_t *);

#endif /* TYPE_H */
