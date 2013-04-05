
#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include "class.h"
#include "dict.h"

typedef signed long builtin_int_t;

struct object
{
    struct class_t      *type;
    union
    {
        struct class_t      *cl;
        struct dict         *members;
        struct method       *method;
        /* Some other types so builtin types can actually store data */
        builtin_int_t       integer;
        double              dbl;
    } v;
};

struct object *object_new(void);

struct object *new_instance(struct class_t *);

struct object *get_member(struct object *, enum membertype, char *);
void set_member(struct object *, enum membertype, char *, struct object *);

#endif

