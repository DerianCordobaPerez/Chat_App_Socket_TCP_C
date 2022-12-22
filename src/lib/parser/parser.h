#include <string.h>
#include "allocator.h"
#include "argument.h"

typedef struct parser {
    const char *(*usage)();
    Argument *argument;
} parser;

parser *alloc();
const parser *Parser();
const char *usage();