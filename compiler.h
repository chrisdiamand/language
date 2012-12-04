
#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"

struct compiler;
struct class_t;

struct compiler *compiler_new(struct scanner_input *);
void compile(struct compiler *, struct class_t *);

#endif

