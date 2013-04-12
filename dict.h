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

#ifndef DICT_H
#define DICT_H

typedef void *dict_value;

struct dict;

typedef void (*dict_print_function_t)(dict_value);

struct dict *dict_new(void);
struct dict *dict_copy(struct dict *);
dict_value dict_get(struct dict *, char *);
void dict_set(struct dict *, char *, dict_value);
void dict_print(struct dict *, dict_print_function_t);

/* For iterating over a dictionary */
void dict_begin(struct dict *);
dict_value dict_next(struct dict *, char **);

#endif

