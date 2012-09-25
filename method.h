
#ifndef FUNCTION_H
#define FUNCTION_H

#include "type.h"

typedef struct object *(*C_function_t)(struct object **);
struct object *method_from_C(C_function_t, const char **);

#endif

