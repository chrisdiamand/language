
#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include "dict.h"

struct method;
struct object;

/* The compiler's representation of a class */
struct class_t
{
    /* The class it inherits from */
    struct class_t      *parent;
    /* Dictionaries containing pointers to struct class_members */
    struct dict         *dynamic_members;
    struct dict         *static_members;
    struct object       *static_value;
    unsigned int        num_static_vals, num_dynamic_vals;
    unsigned int        max_index;
};

struct class_member
{
    struct object       *val;
    /* If this class is a member of another class, what index is it
     * referred to by at run time? */
    unsigned int        index;
};

struct instance
{
    struct class_t      *type;
    struct object       **members;
};

enum object_type
{
    OBJECT_TYPE_NONE,
    OBJECT_TYPE_CLASS,
    OBJECT_TYPE_INSTANCE,
    OBJECT_TYPE_RAW_DATA,
    OBJECT_TYPE_METHOD
};

struct object
{
    enum object_type    type;
    union
    {
        struct class_t      *cl;
        struct instance     *inst;
        struct method       *method;
        /* Some other types so builtin types can actually store data */
        signed long         integer;
        double              dbl;
    } v;
};

struct class_t *class_new(struct class_t *);
void class_print(struct class_t *);
void class_add_static_member(struct class_t *, char *, struct object *);
void class_add_dynamic_member(struct class_t *, char *, struct object *);

struct object *object_new(void);
struct object *object_with_data(void);
struct object *object_from_class(struct class_t *);
struct object *object_from_method(struct method *);

struct object *new_instance(struct class_t *);

#endif

