
#ifndef FUNCTION_H
#define FUNCTION_H

#include "object.h"
#include "state.h"

typedef struct object *(*C_function_t)(struct state *, struct object **);
struct object *method_from_C(struct state *, C_function_t, const char **);

#endif

