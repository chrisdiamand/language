
#include <stdlib.h>

#include "method.h"
#include "type.h"

struct method
{
    struct object *ret;
};

struct object *method_from_C(C_function_t func, const char **types)
{
    struct method *M = NULL;
    return object_from_method(M);
}

