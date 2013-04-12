#ifndef CLASS_H
#define CLASS_H

#include "state.h"

enum membertype
{
    DYNAMIC_MEMBER = 0,
    STATIC_MEMBER = 1
};

/* The compiler's representation of a class */
struct class_t
{
    /* The class it inherits from */
    struct class_t      *parent;
    /* Dictionaries containing pointers to struct class_members.
     * members[DYNAMIC_MEMBER] and members[STATIC_MEMBER] */
    struct dict         *members[2];

    /* Names of types for templates */
    int nparams;
    char **paramnames;
};

struct class_t *class_new(struct class_t *);
struct object *class_new_obj(struct state *, struct class_t *);
struct class_t *class_from_object(struct state *, struct object *);
struct class_t *class_from_type(struct state *, struct type_t *);
void class_print(struct state *, struct class_t *);
void class_add_member(struct class_t *, enum membertype, char *, struct type_t *);
struct type_t *class_get_static_member(struct class_t *C, char *name);

#endif
