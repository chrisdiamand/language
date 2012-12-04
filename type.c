
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gc.h"
#include "type.h"

struct class_t *class_new(struct class_t *parent)
{
    struct class_t *ret = GC_malloc(sizeof(struct class_t));

    ret->parent = parent;
    ret->members[DYNAMIC_MEMBER] = NULL;
    ret->members[STATIC_MEMBER] = NULL;

    return ret;
}

void class_add_member(struct class_t *C, enum mtype sd, char *name, struct object *value)
{
    assert(sd == DYNAMIC_MEMBER || sd == STATIC_MEMBER);

    if (!C->members[sd])
        C->members[sd] = dict_new();

    dict_set(C->members[sd], name, value);
}

void class_print(struct class_t *S)
{
    printf("Scope %p: parent = %p\n", (void *) S, (void *) S->parent);
    printf("Members:\n");
    if (S->members[STATIC_MEMBER])
        dict_print(S->members[STATIC_MEMBER], NULL);

    if (S->members[DYNAMIC_MEMBER])
        dict_print(S->members[DYNAMIC_MEMBER], NULL);
}

struct object *object_new(void)
{
    struct object *ret = GC_malloc(sizeof(struct object));
    ret->type = OBJECT_TYPE_NONE;
    ret->v.inst = NULL;
    return ret;
}

struct object *object_from_class(struct class_t *C)
{
    struct object *O = object_new();
    O->type = OBJECT_TYPE_CLASS;
    O->v.cl = C;
    return O;
}

struct object *object_from_method(struct method *M)
{
    struct object *O = object_new();
    O->type = OBJECT_TYPE_METHOD;
    O->v.method = M;
    return O;
}

struct object *object_with_data(void)
{
    struct object *O = object_new();
    O->type = OBJECT_TYPE_RAW_DATA;
    O->v.integer = 0;
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
    struct instance *I = GC_malloc(sizeof(struct instance));

    ret->type = OBJECT_TYPE_INSTANCE;
    ret->v.inst = I;

    I->type = C;

    /* Make new instances of the dynamic members. */
    I->members = dict_copy(C->members[DYNAMIC_MEMBER]);

    return ret;
}

struct object *get_member(struct object *inst, enum mtype sd, char *name)
{
    struct instance *I = inst->v.inst;
    struct object *O = NULL;
    struct class_t *cl = NULL;
    assert(inst->type == OBJECT_TYPE_INSTANCE);

    switch (sd)
    {
        case DYNAMIC_MEMBER:
            return (struct object *) dict_get(I->members, name);
        case STATIC_MEMBER:
            for (cl = I->type; cl != NULL; cl = cl->parent)
            {
                O = (struct object *) dict_get(cl->members[STATIC_MEMBER], name);
                if (O)
                    return O;
            }
            break;
    }
    return NULL;
}

void set_member(struct object *O, enum mtype sd, char *name, struct object *val)
{
    assert(O->type == OBJECT_TYPE_INSTANCE);
}

