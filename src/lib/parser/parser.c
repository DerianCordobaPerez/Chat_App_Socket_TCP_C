#include "parser.h"

parser *alloc() {
    const size_t size = 1;
    parser *_parser = (parser*)allocator(size);

    return _parser;
}

const parser *Parser() {
    parser *_parser = alloc();
    _parser->usage = &usage;

    return _parser;
}

const char *usage() {
    const size_t size = 255;
    char *parser_options = (char*)allocator(size);

    strcpy(parser_options, "Usage: chat ...");

    return parser_options;
}