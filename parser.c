
#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"

static struct scanner_input *input = NULL;
static struct scanner_token ct;

static void next_token(void)
{
    ct = scan_next(input);
}

