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

#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include "class.h"
#include "dict.h"
#include "method.h"
#include "state.h"

typedef signed long builtin_int_t;

struct object
{
    struct type_t           *type;
    union
    {
        struct class_t      *cl;
        struct dict         *members;
        struct method_t     *method;
        /* Some other types so builtin types can actually store data */
        builtin_int_t       integer;
        double              dbl;
    } v;
};

struct object *object_new(void);

struct object *new_instance(struct state *, struct class_t *);

struct object *get_member(struct state *S, struct object *inst,
                          enum membertype sd, char *name);
void set_member(struct object *, enum membertype, char *, struct object *);

struct object *object_from_class(struct state *, struct class_t *);
struct object *object_from_method(struct state *, struct method_t *);

#endif

