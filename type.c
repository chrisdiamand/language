
#include <stdio.h>
#include <stdlib.h>

#include "gc.h"
#include "type.h"

struct class_t *class_new(struct class_t *parent)
{
    struct class_t *ret = GC_malloc(sizeof(struct class_t));

    ret->parent = parent;
    ret->dynamic_members = ret->static_members = NULL;

    if (parent)
        ret->max_index = parent->max_index;
    else
        ret->max_index = 0;

    return ret;
}

void class_add_dynamic_member(struct class_t *C, char *name, struct object *value)
{
    struct class_member *memb = GC_malloc(sizeof(struct class_member));
    memb->val = value;
    memb->index = 0;

    if (!C->dynamic_members)
        C->dynamic_members = dict_new();

    dict_set(C->dynamic_members, name, memb);
}

void class_add_static_member(struct class_t *C, char *name, struct object *value)
{
    struct class_member *memb = GC_malloc(sizeof(struct class_member));
    memb->val = value;
    memb->index = 0;

    if (!C->static_members)
        C->static_members = dict_new();

    dict_set(C->static_members, name, memb);
}

void class_print(struct class_t *S)
{
    printf("Scope %p: parent = %p\n", (void *) S, (void *) S->parent);
    printf("Members:");
    if (S->static_members)
        dict_print(S->static_members, NULL);
    if (S->dynamic_members)
        dict_print(S->dynamic_members, NULL);
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

    if (!C->num_dynamic_vals)
        I->members = NULL;
    else
        I->members = GC_malloc(sizeof(struct object *) * C->num_dynamic_vals);

    /* Populate the dynamic members of the new instance with objects of the
     * correct types as specified in C and its parents */
    while (C)
    {
        struct class_member *memb = NULL;
        dict_begin(C->dynamic_members);

        while ( (memb = dict_next(C->dynamic_members, NULL)) )
            I->members[memb->index] = object_copy(memb->val);

        C = C->parent;
    }

    return ret;
}

