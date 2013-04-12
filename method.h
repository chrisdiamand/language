#ifndef FUNCTION_H
#define FUNCTION_H

#include "object.h"
#include "state.h"

struct method_t
{
    struct type_t   *returntype;
};

typedef struct object *(*C_function_t)(struct state *, struct object **);
struct type_t *method_from_C(struct state *, C_function_t, const char *);

#endif
