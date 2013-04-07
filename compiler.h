
#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "state.h"

void compile(struct state *, struct scanner_input *, struct class_t *);

#endif

