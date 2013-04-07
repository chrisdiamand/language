
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "class.h"
#include "method.h"
#include "object.h"
#include "state.h"

static struct class_t *class_int = NULL;

static struct object *integer_add(struct state *S, struct object **args)
{
    struct object *lobj = args[0];
    struct object *robj = args[1];

    signed long l = lobj->v.integer;
    signed long r = robj->v.integer;

    struct object *ret = new_instance(class_int);
    
    assert(lobj->type == class_int);
    assert(robj->type == class_int);

    /* The actual addition */
    ret->v.integer = l + r;

    return ret;
}

static struct object *integer_class(struct state *S)
{
    const char *two_op_types[] = {"integer", "integer", "integer", NULL};
    struct object *C = class_new_obj(S, S->class_object);

    /* Actually create the class */
    class_int = C->v.cl;

    class_add_member(class_int, STATIC_MEMBER, "+", method_from_C(S, integer_add, two_op_types));

    return C;
}

void register_builtin_types(struct state *S, struct class_t *N)
{
    class_add_member(N, STATIC_MEMBER, "int", integer_class(S));
    state_class_to_scope(S, "int", class_int);
}

