
#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "gc.h"
#include "scanner.h"

struct compiler
{
    struct scanner_input    *input;
    struct scanner_token    ct;
};

static struct compiler *C = NULL;

static struct scanner_token next_token()
{
    C->ct = scan_next(C->input);
    return C->ct;
}

struct compiler *compiler_new(struct scanner_input *in)
{
    struct compiler *C = GC_malloc(sizeof(struct compiler));
    C->input = in;
    return C;
}

void compile(struct compiler *comp_state)
{
    C = comp_state;
    next_token();
}

