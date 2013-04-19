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
#include <string.h>

#include "class.h"
#include "gc.h"
#include "type.h"

struct type_t *type_new(struct class_t *cl)
{
    struct type_t *T = GC_malloc(sizeof(struct type_t));
    assert(cl);
    if (cl->nparams > 0)
        T->params = GC_malloc(sizeof(struct type_t *) * cl->nparams);
    T->cl = cl;
    return T;
}

struct type_t *type_from_class(struct state *S, struct class_t *cl)
{
    struct type_t *T = type_new(S->class_class);
    T->cl = S->class_class;
    T->v.cl = cl;
    return T;
}

struct type_t *type_from_method(struct state *S, struct method_t *method)
{
    struct type_t *T = type_new(S->class_method);
    T->cl = S->class_method;
    T->v.method = method;
    return T;
}

size_t type_to_string(struct type_t *T, char *buf, size_t stringlen)
{
    size_t pos;
    assert(T);
    assert(T->cl);
    pos = strlen(T->cl->name);
    snprintf(buf, stringlen, "%s", T->cl->name);
    if (T->cl->nparams > 0)
    {
        int i;
        snprintf(buf + pos, stringlen - pos++, "<");
        for (i = 0; i < T->cl->nparams; i++)
            pos += type_to_string(T->params[i], buf + pos, stringlen - pos);
        snprintf(buf + pos, stringlen - pos++, ">");
    }
    return pos;
}
