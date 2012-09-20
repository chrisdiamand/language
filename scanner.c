

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void die(const char *, ...);

#include "gc.h"
#include "scanner.h"

/* This could be done with a table but it's not really worth bothering */
char *scanner_token_name(enum scanner_type t)
{
    /* No default case so the compiler will warn when a type is missing */
    switch (t)
    {
        case TOK_NULL:              return "null";
        case TOK_OPEN_BRACKET:      return "\'(\'";
        case TOK_CLOSED_BRACKET:    return "\')\'";
        case TOK_STARTBLOCK:        return "\'{\'";
        case TOK_ENDBLOCK:          return "\'}\'";
        case TOK_IDENTITY:          return "identity";
        case TOK_INT:               return "integer";
        case TOK_DECIMAL:           return "decimal";
        case TOK_STRING:            return "string";
        case TOK_ASSIGN:            return "assignment";
        case TOK_ISEQUAL:           return "\'==\'";

        case TOK_ADD:               return "\'+\'";
        case TOK_SUB:               return "\'-\'";
        case TOK_MUL:               return "\'*\'";
        case TOK_DIV:               return "\'/\'";

        case TOK_PRINT:             return "\'print\'";
        case TOK_RETURN:            return "\'return\'";
        case TOK_DOT:               return "dot \'.\'";
        case TOK_COMMA:             return "\',\'";
        case TOK_VAR:               return "\'var\'";
        case TOK_EOF:               return "EOF";
    }
    return "<?>";
}

/* Return an input source that reads from a file */
struct scanner_input *scan_input_file(FILE *fp)
{
    struct scanner_input *i = GC_malloc(sizeof(struct scanner_input));

    if (fp == NULL)
    {
        fprintf(stderr, "Null file pointer passed to struct scanner_inputFile\n");
        exit(1);
    }

    i->fp = fp;
    i->filename = "<unknown>";
    i->str = NULL;
    i->pos = 0;
    return i;
}

/* Return an input source that reads from a file */
struct scanner_input *scan_input_filename(char *fname)
{
    FILE *fp = fopen(fname, "r");
    struct scanner_input *i = GC_malloc(sizeof(struct scanner_input));

    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file \'%s\'\n", fname);
        exit(1);
    }

    i->fp = fp;
    i->filename = GC_strdup(fname);
    i->str = NULL;
    i->pos = 0;
    return i;
}


struct scanner_input *scan_input_string(char *s)
{
    struct scanner_input *i = GC_malloc(sizeof(struct scanner_input));

    if (s == NULL)
        die("Null string passed to struct scanner_inputFile()", 1);

    i->str = s;
    i->pos = 0;
    i->fp = NULL;
    return i;
}

/* Reads the next character from the input source */
static char retrieve_next_char(struct scanner_input *I)
{
    if (I->str != NULL)
    {
        char c = I->str[(I->pos)++];
        if (c == '\0')
            return 0;
        return c;
    }
    /* FIXME: Add some sort of buffering instead of calling getc() loads */
    else if (I->fp != NULL)
    {
        int i;
        i = getc(I->fp);
        if (i == EOF)
        {
            return 0;
        }
        return (char) i;
    }
    else
    {
        die("Input source has no sources set", 1);
    }
    return 0;
}

/* Wrapper for retrieve_next_char() that counts the line numbers */
static char next_char(struct scanner_input *I)
{
    char c = retrieve_next_char(I);
    if (c == '\n')
        I->line_number++;
    return c;
}

static void put_back(struct scanner_input *I, char c)
{
    if (c == '\n')
        I->line_number--;
    if (I->str != NULL)
    {
        I->pos =- 1;
    }
    else if (I->fp != NULL)
    {
        /* FIXME: This won't work with a buffer (see above, next_char() ) */
        ungetc(c, I->fp);
    }
}

/* FIXME: Add escape sequences \n etc */
static char *read_quoted_string(struct scanner_input *I)
{
    unsigned int len = 32, pos = 0;
    char *s = GC_malloc(len), c;
    if (s == NULL)
        die("read_quoted_string(): GC_malloc() failed!", 1);
    do
    {
        c = next_char(I);
        if (pos >= len - 1)
        {
            char *NewStr;
            len *= 2;
            NewStr = GC_realloc(s, len);
            if (NewStr == NULL)
                die("read_quoted_string: GC_realloc() failed!", 1);
            s = NewStr;
        }
        s[pos++] = c;
    } while (c != '\"');
    /* Use pos - 1 to remove the final quote */
    s[pos - 1] = '\0';
    return s;
}

struct scanner_token scan_next(struct scanner_input *I)
{
    char c = 1;
    int i; 
    struct scanner_token T;
    while (c != 0)
    {
        c = next_char(I);
        if (isalpha(c)) /* An identifier */
        {
            char name[512]; /* Arbitrary limit to name length */
            name[0] = c;
            i = 1;
            c = next_char(I);
            while (isalnum(c) || c == '_')
            {
                name[i++] = c;
                c = next_char(I);
            }
            name[i] = 0;
            put_back(I, c);
            if      ( strcmp(name, "var") == 0 )        {   T.type = TOK_VAR;       }
            else if ( strcmp(name, "print") == 0 )      {   T.type = TOK_PRINT;     }
            else if ( strcmp(name, "return") == 0 )     {   T.type = TOK_RETURN;    }
            else
            {
                T.type = TOK_IDENTITY;
                T.value = GC_strdup(name);
            }
            return T;
        }
        /* FIXME: Need to check that there is only 1 '.' and 'e' in a number.
         * Also maybe make 'e' an actual operator instead because that would be cool */
        if (isdigit(c))
        {
            char num[512];
            int ndots = 0;
            num[0] = c;
            i = 1;
            c = next_char(I);
            while (isdigit(c) || c == '.')
            {
                num[i++] = c;
                if (c == '.')
                    ndots++;
                c = next_char(I);
            }
            num[i] = 0;
            put_back(I, c);
            if (ndots > 1)
                die("Invalid number: Too many decimal points", 1);
            T.type = (ndots == 0) ? TOK_INT : TOK_DECIMAL;
            /* It must be strdup'd to stop it being destroyed as soon as this loop returns */
            T.value = GC_strdup(num);
            return T;
        }
        if (c == '=')
        {
            T.type = TOK_ASSIGN;
            /* Check it is =, not == */
            c = next_char(I);
            if (c == '=') /* It must be comparison: == */
                T.type = TOK_ISEQUAL;
            else
                put_back(I, c);
            return T;
        }
        if (c == '\"')
        {
            char *s = read_quoted_string(I);
            T.value = s;
            T.type = TOK_STRING;
            return T;
        }
        if (c == '#')
        {
            while (c != '\n')
                c = next_char(I);
        }
        switch (c)
        {
            case '.':   T.type = TOK_DOT;           return T;
            case '+':   T.type = TOK_ADD;           return T;
            case '-':   T.type = TOK_SUB;           return T;
            case '*':   T.type = TOK_MUL;           return T;
            case '/':   T.type = TOK_DIV;           return T;
            case '(':   T.type = TOK_OPEN_BRACKET;  return T;
            case ')':   T.type = TOK_CLOSED_BRACKET;return T;
            case '{':   T.type = TOK_STARTBLOCK;    return T;
            case '}':   T.type = TOK_ENDBLOCK;      return T;
            case ',':   T.type = TOK_COMMA;         return T;
            default:    break;
        }
    }
    T.type = TOK_EOF;
    return T;
}

