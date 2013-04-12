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
    char *name;
    /* The class it inherits from */
    struct class_t      *parent;
    /* Dictionaries containing pointers to struct class_members.
     * members[DYNAMIC_MEMBER] and members[STATIC_MEMBER] */
    struct dict         *members[2];

    /* Names of types for templates */
    int nparams;
    char **paramnames;
};

struct class_t *class_new(char *, struct class_t *);
struct class_t *class_from_type(struct state *, struct type_t *);
void class_print(struct state *, struct class_t *);
void class_add_member(struct class_t *, enum membertype, char *, struct type_t *);
struct type_t *class_get_static_member(struct class_t *C, char *name);

#endif

