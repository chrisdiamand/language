
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "dict.h"
#include "scanner.h"
#include "type.h"

void register_builtin_types(struct class_t *);

void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    exit(1);
}

int main(int argc, char **argv)
{
    struct scanner_input *I = NULL;
    struct compiler *C = NULL;
    struct class_t *main_namespace = class_new(NULL);

    if (argc > 1)
        I = scan_input_filename(argv[1]);
    else
        I = scan_input_file(stdin);

    register_builtin_types(main_namespace);
    class_print(main_namespace);

    C = compiler_new(I);
    compile(C, main_namespace);

    return 0;
}

