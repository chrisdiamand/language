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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "class.h"
#include "src_to_pt.h"
#include "dict.h"
#include "gc.h"
#include "object.h"
#include "run.h"
#include "scanner.h"
#include "stack.h"

void register_builtin_types(struct state *, struct class_t *);

void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    exit(1);
}

int main(int argc, char **argv)
{
    struct scanner_token *in = NULL;
    struct state *S = state_new();
    struct class_t *main_namespace = class_new("__main__", NULL);

    if (argc > 1)
        in = scan_filename(argv[1]);
    else
        in = scan_file(stdin);

    register_builtin_types(S, main_namespace);
    compile(S, in, main_namespace);

    class_print(S, main_namespace);

    run_class(main_namespace);

    return 0;
}

