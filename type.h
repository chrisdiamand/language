
#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include "dict.h"

enum mtype
{
    DYNAMIC_MEMBER = 0,
    STATIC_MEMBER = 1
};

struct method;
struct object;

/* The compiler's representation of a class */
struct class_t
{
    /* The class it inherits from */
    struct class_t      *parent;
    /* Dictionaries containing pointers to struct class_members.
     * members[DYNAMIC_MEMBER] and members[STATIC_MEMBER] */
    struct dict         *members[2];
};

struct instance
{
    struct class_t      *type;
    /* List of dynamic members */
    struct dict         *members;
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
void class_add_member(struct class_t *, enum mtype, char *, struct object *);

struct object *object_new(void);
struct object *object_with_data(void);
struct object *object_from_class(struct class_t *);
struct object *object_from_method(struct method *);

struct object *new_instance(struct class_t *);

struct object *get_member(struct object *, enum mtype, char *);
void set_member(struct object *, enum mtype, char *, struct object *);

#endif

