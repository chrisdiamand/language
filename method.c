
#include <stdlib.h>

#include "method.h"
#include "object.h"
#include "state.h"
#include "type.h"

struct type_t *method_from_C(struct state *S, C_function_t func, const char *decl)
{
    struct method_t *M = NULL;
    return type_from_method(S, M);
}

