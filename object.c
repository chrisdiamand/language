#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gc.h"
#include "object.h"
#include "state.h"

struct object *object_new(void)
{
    struct object *ret = GC_malloc(sizeof(struct object));
    ret->type = NULL;
    ret->v.members = NULL;
    return ret;
}

struct object *object_from_class(struct state *S, struct class_t *cl)
{
    struct object *O = object_new();
    O->type = S->class_class;
    O->v.cl = cl;
    return O;
}

struct object *object_from_method(struct state *S, struct method *M)
{
    struct object *O = object_new();
    O->type = S->class_method;
    O->v.method = M;
    return O;
}

struct object *object_copy(struct object *O)
{
    struct object *ret = object_new();
    *ret = *O;
    return ret;
}

struct object *new_instance(struct class_t *C)
{
    struct object *ret = object_new();

    ret->type = C;

    /* Make new instances of the dynamic members. */
    ret->v.members = dict_copy(C->members[DYNAMIC_MEMBER]);

    return ret;
}

struct object *get_member(struct state *S, struct object *inst,
                          enum membertype sd, char *name)
{
    struct object *O = NULL;
    struct class_t *cl = NULL;
    assert(inst->type != S->class_int
        && inst->type != S->class_double
        && inst->type != S->class_method
        && inst->type != S->class_class);

    switch (sd)
    {
        case DYNAMIC_MEMBER:
            return (struct object *) dict_get(inst->v.members, name);
        case STATIC_MEMBER:
            for (cl = inst->type; cl != NULL; cl = cl->parent)
            {
                O = (struct object *) dict_get(cl->members[STATIC_MEMBER], name);
                if (O)
                    return O;
            }
            break;
    }
    return NULL;
}

/* Is this even needed? */
void set_member(struct object *O, enum membertype sd, char *name, struct object *val)
{
}
