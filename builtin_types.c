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
#include "method.h"
#include "object.h"
#include "state.h"

static struct class_t *class_int = NULL;

static struct object *integer_add(struct state *S, struct object **args)
{
    struct object *lobj = args[0];
    struct object *robj = args[1];

    signed long l = lobj->v.integer;
    signed long r = robj->v.integer;

    struct object *ret = new_instance(S, class_int);
    
    assert(lobj->type->cl == class_int);
    assert(robj->type->cl == class_int);

    /* The actual addition */
    ret->v.integer = l + r;

    return ret;
}

static struct type_t *integer_class(struct state *S)
{
    /* Actually create the class */
    class_int = class_new("int", S->class_object);

    class_add_member(class_int, STATIC_MEMBER, "_add_",
                     method_from_C(S, integer_add, "int _add_(int a)"));

    return type_from_class(S, class_int);
}

void register_builtin_types(struct state *S, struct class_t *N)
{
    class_add_member(N, STATIC_MEMBER, "int", integer_class(S));
}

