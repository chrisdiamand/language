
#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "gc.h"
#include "scanner.h"
#include "stack.h"

struct compiler
{
    struct scanner_input    *input;
    struct scanner_token    ct;
    struct stack            *scope;
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
    C->scope = stack_new();
    return C;
}

static struct object *class_declaration(void)
{
    return NULL;
}

static struct object *function_declaration(void)
{
    return NULL;
}

/* Parse a list of declarations (variables, methods and classes)
 * and add them to namespace */
static void declaration_list(void)
{
    struct object *obj = NULL;
    switch (C->ct.type)
    {
        case TOK_CLASS:
            obj = class_declaration();
            break;
        case TOK_FUNCTION:
            obj = function_declaration(); 
            break;
        default:
            break;
    }
}

void compile(struct compiler *comp_state, struct class_t *main)
{
    C = comp_state;
    next_token();
    declaration_list();
}

