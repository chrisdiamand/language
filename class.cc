/*
 * Copyright (c) 2013 Chris Diamand
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "class.h"
#include "gc.h"
#include "object.h"

struct class_t *class_from_object(struct state *S, struct object *obj)
{
    if (obj == NULL)
        return NULL;
    if (obj->type->cl != S->class_class)
        return NULL;
    return obj->v.cl;
}

struct class_t *class_from_type(struct state *S, struct type_t *T)
{
    if (T == NULL)
        return NULL;
    if (T->cl != S->class_class)
        return NULL;
    return T->v.cl;
}

/* Create a new class inheriting from parent */
struct class_t *class_new(char *name, struct class_t *parent)
{
    struct class_t *cl = GC_malloc(sizeof(struct class_t));

    cl->name = name;
    cl->parent = parent;
    cl->members[DYNAMIC_MEMBER] = dict_new();
    cl->members[STATIC_MEMBER] = dict_new();
    cl->nparams = 0;
    cl->paramnames = NULL;

    return cl;
}

void class_print(struct state *S, struct class_t *C)
{
    struct dict_pair *pair = NULL;

    printf("Class %p extends parent %p\n", (void *) C, (void *) C->parent);

    for (pair = dict_begin(C->members[STATIC_MEMBER]);
         pair != NULL;
         pair = dict_next(C->members[STATIC_MEMBER]))
    {
        char buf[512];
        type_to_string((struct type_t *) pair->value, buf, sizeof(buf));
        printf("    static %s %s;\n", buf, pair->key);
    }

    for (pair = dict_begin(C->members[DYNAMIC_MEMBER]);
         pair != NULL;
         pair = dict_next(C->members[DYNAMIC_MEMBER]))
    {
        char buf[512];
        type_to_string((struct type_t *) pair->value, buf, sizeof(buf));
        printf("    %s %s;\n", buf, pair->key);
    }
}

void class_add_member(struct class_t *C, enum membertype sd, char *name, struct type_t *value)
{
    assert(sd == DYNAMIC_MEMBER || sd == STATIC_MEMBER);
    assert(C->members[sd]);
    dict_set(C->members[sd], name, (void *) value);
}

struct type_t *class_get_static_member(struct class_t *C, char *name)
{
    while (C)
    {
        struct type_t *ret;
        if (!C->members[STATIC_MEMBER])
            ret = NULL;
        else
            ret = (struct type_t *) dict_get(C->members[STATIC_MEMBER], name);

        if (ret)
            return ret;

        C = C->parent;
    }
    return NULL;
}

