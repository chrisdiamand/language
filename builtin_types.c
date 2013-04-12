
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

    struct object *ret = new_instance(S, class_int);
    
    assert(lobj->type->cl == class_int);
    assert(robj->type->cl == class_int);

    /* The actual addition */
    ret->v.integer = l + r;

    return ret;
}

static struct type_t *integer_class(struct state *S)
{
    /* Actually create the class */
    class_int = class_new("int", S->class_object);

    class_add_member(class_int, STATIC_MEMBER, "_add_",
                     method_from_C(S, integer_add, "int _add_(int a)"));

    return type_from_class(S, class_int);
}

void register_builtin_types(struct state *S, struct class_t *N)
{
    class_add_member(N, STATIC_MEMBER, "int", integer_class(S));
}

