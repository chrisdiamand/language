
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "dict.h"
#include "scanner.h"

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
    if (argc > 1)
        I = scan_input_filename(argv[1]);
    else
        I = scan_input_file(stdin);
    return 0;
}

