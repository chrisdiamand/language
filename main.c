#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "class.h"
#include "compiler.h"
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
