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

#ifndef TYPE_H
#define TYPE_H

#include "class.h"
#include "state.h"

struct state;

struct type_t
{
    struct class_t      *cl;
    struct type_t       **params;
    union
    {
        struct class_t      *cl;
        struct method_t     *method;
    } v;
};

struct type_t *type_new(struct class_t *);
struct type_t *type_from_class(struct state *, struct class_t *);
struct type_t *type_from_method(struct state *, struct method_t *);

#endif /* TYPE_H */

