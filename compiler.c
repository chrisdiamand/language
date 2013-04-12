#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "class.h"
#include "compiler.h"
#include "dict.h"
#include "gc.h"
#include "object.h"
#include "scanner.h"
#include "stack.h"
#include "state.h"
#include "type.h"

struct compiler
{
    struct scanner_token    *tokens;
    unsigned int            tokenpos;
    struct state            *S;
};

static struct compiler *C = NULL;
static struct scanner_token ct;

static struct scanner_token next_token()
{
    ct = C->tokens[C->tokenpos];
    if (ct.type != TOK_EOF)
        C->tokenpos++;
    return ct;
}

static void syntaxerror(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Syntax error, line %u: ", ct.line);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fflush(stderr);
}

static int expect(enum scanner_type tok)
{
    if (ct.type != tok)
    {
        syntaxerror("Expected %s, got %s",
                    scanner_token_name(tok),
                    scanner_token_name(ct.type));
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
        typenames[i] = ct.value;
        expect(TOK_IDENTIFIER);

        if (ct.type != TOK_COMMA)
            break;
        expect(TOK_COMMA);
    }
    cl->nparams = i;
    cl->paramnames = GC_malloc(sizeof(char *) * cl->nparams);
    memcpy(cl->paramnames, typenames, sizeof(char *) * cl->nparams);
}

static void class_body(struct class_t *, enum scanner_type);

/* Parse a class declaration and add it to namespace */
static void class_declaration(struct class_t *namespace)
{
    char *classname = NULL;
    /* FIXME: Inheritance syntax */
    struct class_t *cl = class_new(C->S->class_object);

    expect(TOK_CLASS);
    classname = ct.value;
    expect(TOK_IDENTIFIER);

    /* Add to the namespace/outer class so it can reference itself */
    class_add_member(namespace, STATIC_MEMBER, classname,
                     type_from_class(C->S, cl));

    /* Increase the scope level */
    stack_push(C->S->typescope, cl);

    /* Template typename list */
    if (ct.type == TOK_LT)
    {
        expect(TOK_LT);
        class_typename_list(cl);
        expect(TOK_GT);
    }

    /* The actual class body */
    class_body(cl, TOK_END);
    expect(TOK_END);
}

static struct type_t *type_specifier(void)
{
    char *typename = ct.value;
    struct class_t *cl;
    struct type_t *ret = NULL, *class_type;
    expect(TOK_IDENTIFIER);
    class_type = state_lookup_global(C->S, typename);

    if (!class_type)
    {
        fprintf(stderr, "No such type %s\n", typename);
        return NULL;
    }
    if (class_type->cl != C->S->class_class)
    {
        fprintf(stderr, "\'%s\' used in declaration but isn't of type \'class\'\n",
                typename);
        return NULL;
    }

    /* Allow inner classes e.g. 'module.SomeType x;' */
    while (ct.type == TOK_DOT)
    {
        char *innername;
        struct class_t *innercl;
        expect(TOK_DOT);
        innername = ct.value;
        expect(TOK_IDENTIFIER);

        innercl = class_from_type(C->S, class_get_static_member(cl, innername));
        if (!innercl)
        {
            fprintf(stderr, "Error: Class %s has no inner class %s\n",
                            typename, innername);
            return NULL;
        }
        typename = innername;
        cl = innercl;
    }

    ret = type_new(cl);

    if (cl->nparams > 0)
    {
        expect(TOK_LT); // FIXME: Bit hacky...
        for (int i = 0; i < cl->nparams; i++)
        {
            ret->params[i] = type_specifier();
            if (i < cl->nparams - 1)
                expect(TOK_COMMA);
        }
        expect(TOK_GT);
    }

    return ret;
}

/* Parse a function/variable declaration. If it is syntactically
 * correct, add it to namespace, otherwise return false. */
static void function_or_variable_declaration(struct class_t *namespace)
{
    struct type_t *type = type_specifier();
    if (!type)
        return;
}

/* Parse a list of declarations (variables, methods and classes)
 * and add them to namespace/class N */
static void class_body(struct class_t *N, enum scanner_type finish)
{
    int errorcount = 0;

    while (ct.type != finish && ct.type != TOK_EOF)
    {
        if (ct.type == TOK_CLASS)
        {
            class_declaration(N);
            errorcount = 0;
        }
        else if (ct.type == TOK_IDENTIFIER)
        {
            function_or_variable_declaration(N);
            errorcount = 0;
        }
        else if (ct.type == TOK_SEMICOLON)
        {
            expect(TOK_SEMICOLON);
        }
        else
        {
            if (errorcount++ == 0)
                syntaxerror("Expected declaration, got %s",
                            scanner_token_name(ct.type));
            next_token();
        }
    }
}

void compile(struct state *S, struct scanner_token *tokens, struct class_t *namespace)
{
    C = GC_malloc(sizeof(struct compiler));
    C->tokens = tokens;
    C->tokenpos = 0;
    C->S = S;
    /* Add the namespace to the outer level of scope */
    stack_push(S->typescope, namespace);

    next_token();

    class_body(namespace, TOK_EOF);
}
