#include <regex.h>
#include <stdbool.h> 
#include "argument.h"

static const Argument *argument;
static regex_t short_option_regex, large_option_regex;

const void transform() {
    regcomp(&short_option_regex, argument->info->_short, 0);
    bool isValidShortArgument = regexec(&short_option_regex, "-[:lower:]", null, 0);

    if(! isValidShortArgument) {
        fail("The option argument is a not valid.");
    }
}