
#include <stdlib.h>

#include "method.h"
#include "object.h"
#include "state.h"

struct method
{
    struct object *ret;
};

struct object *method_from_C(struct state *S, C_function_t func, const char **types)
{
    struct method *M = NULL;
    return object_from_method(S, M);
}

