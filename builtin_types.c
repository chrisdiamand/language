
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "method.h"
#include "type.h"

static struct class_t *int_class = NULL;

static struct object *integer_add(struct object **args)
{
    struct object *lobj = get_member(args[0], DYNAMIC_MEMBER, "raw_int_val");
    struct object *robj = get_member(args[1], DYNAMIC_MEMBER, "raw_int_val");

    signed long l = lobj->v.integer;
    signed long r = robj->v.integer;

    struct object *ret = new_instance(int_class);
    struct object *raw_int_val = object_with_data();
    
    assert(lobj->type == OBJECT_TYPE_RAW_DATA);
    assert(robj->type == OBJECT_TYPE_RAW_DATA);

    /* The actual addition */
    raw_int_val->v.integer = l + r;

    set_member(ret, DYNAMIC_MEMBER, "raw_int_val", raw_int_val);
    return ret;
}

static void add_integer_class(struct class_t *namespace)
{
    const char *two_op_types[] = {"integer", "integer", "integer", NULL};
    struct class_t *C = class_new(NULL);
    struct object *ret = object_from_class(C);

    /* Add the 'integer' class to the namespace. This has to be
     * done before the members are added as they will need to reference
     * the class name in their type declarations. */
    class_add_member(namespace, STATIC_MEMBER, "integer", ret);

    /* Actually create the class */
    int_class = C;

    class_add_member(C, STATIC_MEMBER, "+", method_from_C(integer_add, two_op_types));
    class_add_member(C, DYNAMIC_MEMBER, "raw_int_val", object_with_data());
}

void register_builtin_types(struct class_t *namespace)
{
    add_integer_class(namespace);
}

