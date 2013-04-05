#include <assert.h>
#include <stdlib.h>

#include "class.h"
#include "dict.h"
#include "gc.h"
#include "object.h"

/* The class used to identify class objects as classes */
static struct class_t *class_class = NULL;

static int check(struct object *o)
{
    assert(o->type == class_class);
    return o->type == class_class;
}

/* Create a new class inheriting from parent */
struct object *class_new(struct class_t *parent)
{
    struct object *ret = object_new();
    struct class_t *cl = GC_malloc(sizeof(struct class_t));

    cl->parent = parent;
    cl->members[DYNAMIC_MEMBER] = NULL;
    cl->members[STATIC_MEMBER] = NULL;

    ret->type = class_class;
    ret->v.cl = cl;
    return ret;
}

void class_print(struct object *);
void class_add_member(struct object *, enum membertype, char *, struct object *);


