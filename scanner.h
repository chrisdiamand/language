#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

enum scanner_type
{
    TOK_NULL,

    TOK_OPEN_BRACKET,   /* ( */
    TOK_CLOSED_BRACKET, /* ) */
    //TOK_STARTBLOCK,     /* { */
    //TOK_ENDBLOCK,       /* } */

    TOK_IDENTIFIER,
    TOK_INT,
    TOK_DECIMAL,
    TOK_STRING,

    TOK_ASSIGN,         /* = */

    TOK_ISEQUAL,        /* == */
    TOK_LT,             /* < */
    TOK_LTE,            /* <= */
    TOK_GT,             /* > */
    TOK_GTE,            /* >= */

    TOK_ADD,            /* + */
    TOK_SUB,            /* - */
    TOK_MUL,            /* * */
    TOK_DIV,            /* / */

    TOK_CLASS,          /* 'class' */
    TOK_END,            /* 'end' */
    TOK_FUNCTION,       /* 'function' */
    TOK_RETURN,         /* 'return' */
    TOK_VAR,            /* 'var' */
    TOK_DOT,            /* . */
    TOK_COMMA,          /* , */
    TOK_SEMICOLON,      /* ; */
    TOK_EOF             /* End of file */
};

struct scanner_token
{
    char                *value;
    enum scanner_type   type;
    unsigned int        line;
};

char *scanner_token_name(enum scanner_type);

struct scanner_token *scan_file(FILE *);
struct scanner_token *scan_filename(char *);
struct scanner_token *scan_string(char *);

#endif
