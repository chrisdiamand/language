#ifndef CLASS_H
#define CLASS_H

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
};

struct object *class_new(struct class_t *);
void class_print(struct object *);
void class_add_member(struct object *, enum membertype, char *, struct object *);


#endif
