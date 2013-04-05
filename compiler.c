
#include <stdio.h>
#include <stdlib.h>

#include "class.h"
#include "compiler.h"
#include "gc.h"
#include "scanner.h"
#include "stack.h"
#include "state.h"

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

static void expect(enum scanner_type tok)
{
    if (C->ct.type != tok)
    {
        fprintf(stderr, "Syntax error: Expected %s, got %s\n",
                scanner_token_name(tok),
                scanner_token_name(C->ct.type));
        exit(1);
    }
    next_token();
}

static struct object *class_declaration(void)
{
    expect(TOK_CLASS);
    return NULL;
}

static struct object *function_declaration(void)
{
    expect(TOK_FUNCTION);
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

struct object *compile(struct state *S, struct scanner_input *in)
{
    C = GC_malloc(sizeof(struct compiler));
    C->input = in;

    struct object *ret = class_new(S->class_object);

    next_token();
    declaration_list();
}

