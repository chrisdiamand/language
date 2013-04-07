#include <assert.h>
#include <stdlib.h>

#include "class.h"
#include "dict.h"
#include "gc.h"
#include "object.h"

struct class_t *class_get(struct state *S, struct object *obj)
{
    if (obj == NULL)
        return NULL;
    if (obj->type != S->class_class)
        return NULL;
    return obj->v.cl;
}

/* Create a new class inheriting from parent */
struct class_t *class_new(struct class_t *parent)
{
    struct class_t *cl = GC_malloc(sizeof(struct class_t));

    cl->parent = parent;
    cl->members[DYNAMIC_MEMBER] = NULL;
    cl->members[STATIC_MEMBER] = NULL;
    cl->ntypenames = 0;
    cl->typenames = NULL;

    return cl;
}

struct object *class_new_obj(struct state *S, struct class_t *parent)
{
    struct object *ret = object_new();

    ret->type = S->class_class;
    ret->v.cl = class_new(parent);

    return ret;
}

void class_print(struct state *S, struct class_t *C)
{
    printf("Class %p extends parent %p\n", (void *) C, (void *) C->parent);
    printf("Members:\n");

    if (C->members[STATIC_MEMBER])
        dict_print(C->members[STATIC_MEMBER], NULL);

    if (C->members[DYNAMIC_MEMBER])
        dict_print(C->members[DYNAMIC_MEMBER], NULL);
}

void class_add_member(struct class_t *C, enum membertype sd, char *name, struct object *value)
{
    assert(sd == DYNAMIC_MEMBER || sd == STATIC_MEMBER);

    if (!C->members[sd])
        C->members[sd] = dict_new();

    dict_set(C->members[sd], name, value);
}
