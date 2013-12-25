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

#include <assert.h>
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
    struct type_t           *lookeduptype;
};

static struct compiler *C = NULL;
static struct scanner_token ct;

static struct scanner_token next_token()
{
    ct = C->tokens[C->tokenpos];
    if (ct.type != TOK_EOF)
        C->tokenpos++;

    /* Look through the scopes to see if it's the name of a class */
    C->lookeduptype = NULL;
    if (ct.type == TOK_IDENTIFIER && ct.value)
    {
        C->lookeduptype = state_lookup_global(C->S, ct.value);
        if (C->lookeduptype)
        {
            if (C->lookeduptype->cl == C->S->class_class) // It's a class name
                ct.type = TOK_CLASSNAME;
        }
    }

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

static void expectederror(char *expected)
{
    if (ct.type == TOK_IDENTIFIER)
        syntaxerror("Expected %s, got %s \'%s\'",
                    expected, scanner_token_name(ct.type), ct.value);
    else
        syntaxerror("Expected %s, got %s",
                    expected, scanner_token_name(ct.type));
}

static int expect(enum scanner_type tok)
{
    if (ct.type != tok)
    {
        expectederror(scanner_token_name(tok));
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
    // i isn't incremented at the end of the for loop as it terminates
    // because of 'break;', not the loop condition. Increase it here:
    cl->nparams = i + 1;
    cl->paramnames = GC_malloc(sizeof(char *) * cl->nparams);
    memcpy(cl->paramnames, typenames, sizeof(char *) * cl->nparams);
}

static void class_body(struct class_t *, enum scanner_type);

/* Parse a class declaration and add it to namespace */
static void class_declaration(struct class_t *namespace)
{
    char *classname = NULL;
    /* FIXME: Inheritance syntax */
    struct class_t *cl = NULL;
    expect(TOK_CLASS);
    classname = ct.value;
    expect(TOK_IDENTIFIER);

    cl = class_new(classname, C->S->class_object);

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
    struct class_t *cl = NULL;
    struct type_t *ret = NULL, *class_type;
    class_type = C->lookeduptype;
    expect(TOK_CLASSNAME);
    cl = class_from_type(C->S, class_type);
    assert(cl);

    /* Allow inner classes e.g. 'module.SomeType x;' */
    while (ct.type == TOK_DOT)
    {
        char *innername;
        struct class_t *innercl;
        expect(TOK_DOT);
        innername = ct.value;
        expect(TOK_CLASSNAME);

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

static void method_definition(struct class_t *namespace,
                              struct type_t *return_type,
                              char *methodname)
{
    expect(TOK_OPEN_BRACKET);
    expect(TOK_CLOSED_BRACKET);
    expect(TOK_END);
}

/* Parse a function/variable declaration. If it is syntactically
 * correct, add it to namespace, otherwise return false. */
static void function_or_variable_declaration(struct class_t *namespace)
{
    char *varname;
    struct type_t *type = type_specifier();
    if (!type)
        return;

    varname = ct.value;
    if (!expect(TOK_IDENTIFIER))
        return;

    if (ct.type == TOK_OPEN_BRACKET)
    {
        method_definition(namespace, type, varname);
        return;
    }

    /* Add a list of identifiers to the namespace */
    class_add_member(namespace, DYNAMIC_MEMBER, varname, type);
    while (ct.type == TOK_COMMA)
    {
        expect(TOK_COMMA);
        varname = ct.value;
        if (!expect(TOK_IDENTIFIER))
            break;
        class_add_member(namespace, DYNAMIC_MEMBER, varname, type);
    }
    expect(TOK_SEMICOLON);
}

/* Parse a list of declarations (variables, methods and classes)
 * and add them to namespace/class N */
static void class_body(struct class_t *N, enum scanner_type finish)
{
    int errorcount = 0;

    while (ct.type != finish && ct.type != TOK_EOF)
    {
        switch (ct.type)
        {
            case TOK_CLASS:
                class_declaration(N);
                errorcount = 0;
                break;
            case TOK_CLASSNAME:
                function_or_variable_declaration(N);
                errorcount = 0;
                break;
            case TOK_SEMICOLON:
                expect(TOK_SEMICOLON);
                break;
            default:
                if (errorcount++ == 0)
                    expectederror("declaration");
                next_token();
                break;
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

