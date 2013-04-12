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

#ifndef STATE_H
#define STATE_H

#include "type.h"

struct state
{
    /* Base classes */
    struct class_t          *class_object;
    struct class_t          *class_class;
    struct class_t          *class_method;
    /* Primitives */
    struct class_t          *class_int;
    struct class_t          *class_double;

    /* Stack of classes to know which classes are visible
     * in the current scope during the first pass */
    struct stack            *typescope;
};

struct state *state_new(void);
struct type_t *state_lookup_global(struct state *, char *);

#endif

