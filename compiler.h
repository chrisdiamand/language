
#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "state.h"

struct object *compile(struct state *S, struct scanner_input *in);

#endif

