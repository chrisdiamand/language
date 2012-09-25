
#include <stdio.h>
#include <stdlib.h>

#include "method.h"
#include "type.h"

static struct class_t *int_class = NULL;

static struct object *integer_add(struct object **args)
{
    struct instance *l_inst = args[0]->v.inst, *r_inst = args[1]->v.inst;
    signed long l = l_inst->members[0]->v.integer;
    signed long r = r_inst->members[0]->v.integer;

    struct object *ret = new_instance(int_class);
    ret->v.inst->members[0]->v.integer = l + r;
    return ret;
}

static void add_integer_class(struct class_t *namespace)
{
    static const char *two_op_types[] = {"integer", "integer", "integer", NULL};
    struct class_t *C = class_new(NULL);
    struct object *ret = object_from_class(C);

    class_add_static_member(namespace, "integer", ret);

    int_class = C;

    class_add_static_member(C, "+", method_from_C(integer_add, two_op_types));
    class_add_dynamic_member(C, "val", object_with_data());
}

void register_builtin_types(struct class_t *namespace)
{
    add_integer_class(namespace);
}

