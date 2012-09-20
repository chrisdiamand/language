
#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

enum scanner_type
{
    TOK_NULL,

    TOK_OPEN_BRACKET,   /* ( */
    TOK_CLOSED_BRACKET, /* ) */
    TOK_STARTBLOCK,     /* { */
    TOK_ENDBLOCK,       /* } */

    TOK_IDENTITY,
    TOK_INT,
    TOK_DECIMAL,
    TOK_STRING,

    TOK_ASSIGN,

    TOK_ISEQUAL,

    TOK_ADD,
    TOK_SUB,
    TOK_MUL,
    TOK_DIV,

    TOK_PRINT,
    TOK_VAR,
    TOK_RETURN,
    TOK_DOT,
    TOK_COMMA,
    TOK_EOF
};

struct scanner_token
{
    enum scanner_type   type;
    char                *value;
};

struct scanner_input
{
    FILE *fp;
    char *filename;
    char *str;
    unsigned long int pos, line_number;
};

char *scanner_token_name(enum scanner_type);

struct scanner_input *scan_input_file(FILE *);
struct scanner_input *scan_input_filename(char *);
struct scanner_input *scan_input_string(char *);

struct scanner_token scan_next(struct scanner_input *);

#endif

