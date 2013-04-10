#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "class.h"
#include "compiler.h"
#include "dict.h"
#include "gc.h"
#include "object.h"
#include "scanner.h"
#include "stack.h"
#include "state.h"

struct compiler
{
    struct scanner_input    *input;
    struct scanner_token    ct;

    struct state            *S;
};

static struct compiler *C = NULL;

static struct scanner_token next_token()
{
    C->ct = scan_next(C->input);
    return C->ct;
}

static void syntaxerror(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Syntax error, line %lu: ", C->input->line_number);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
}

static int expect(enum scanner_type tok)
{
    if (C->ct.type != tok)
    {
        syntaxerror("Expected %s, got %s",
                    scanner_token_name(tok),
                    scanner_token_name(C->ct.type));
        next_token();
        return 0;
    }
    next_token();
    return 1;
}

static void class_typename_list(struct class_t *cl)
{
    char *typenames[16]; /* Who needs more than that? */
    int i;
    for (i = 0; i < sizeof(typenames) / sizeof(char *); i++)
    {
        typenames[i] = C->ct.value;
        expect(TOK_IDENTIFIER);

        /* Add the typename to the scope of the current class */
        state_class_to_scope(C->S, typenames[i], C->S->class_object);

        if (C->ct.type != TOK_COMMA)
            break;
        expect(TOK_COMMA);
    }
    cl->ntypenames = i;
    cl->typenames = GC_malloc(sizeof(char *) * cl->ntypenames);
    memcpy(cl->typenames, typenames, sizeof(char *) * cl->ntypenames);
}

static void class_body(struct class_t *, enum scanner_type);

/* Parse a class declaration and add it to namespace */
static void class_declaration(struct class_t *namespace)
{
    char *classname = NULL;
    /* FIXME: Inheritance syntax */
    struct class_t *cl = class_new(C->S->class_object);

    expect(TOK_CLASS);
    classname = C->ct.value;
    expect(TOK_IDENTIFIER);

    /* Add it to the scope */
    state_class_to_scope(C->S, classname, cl);
    /* And the namespace/outer class */
    class_add_member(namespace, STATIC_MEMBER, classname,
                     object_from_class(C->S, cl));

    /* Increase the scope level */
    stack_push(C->S->typescope, dict_new());

    /* Template typename list */
    if (C->ct.type == TOK_LT)
    {
        expect(TOK_LT);
        class_typename_list(cl);
        expect(TOK_GT);
    }

    /* The actual class body */
    expect(TOK_STARTBLOCK);
    class_body(cl, TOK_ENDBLOCK);
    expect(TOK_ENDBLOCK);
}

/* Parse a function declaration and add it to namespace */
static struct object *function_declaration(struct class_t *namespace)
{
    expect(TOK_FUNCTION);
    return NULL;
}

/* Parse a list of declarations (variables, methods and classes)
 * and add them to namespace/class N */
static void class_body(struct class_t *N, enum scanner_type finish)
{
    int errorcount = 0;

    while (C->ct.type != finish && C->ct.type != TOK_EOF)
    {
        switch (C->ct.type)
        {
            case TOK_CLASS:
                class_declaration(N);
                errorcount = 0;
                break;
            case TOK_FUNCTION:
                function_declaration(N); 
                errorcount = 0;
                break;
            default:
                if (errorcount++ == 0)
                    syntaxerror("Expected declaration, got %s",
                                scanner_token_name(C->ct.type));

                next_token();
                break;
        }
    }
}

void compile(struct state *S, struct scanner_input *in, struct class_t *namespace)
{
    C = GC_malloc(sizeof(struct compiler));
    C->input = in;
    C->S = S;

    next_token();

    class_body(namespace, TOK_EOF);
}

