#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include "ST_stack.h"
#include "scanner.h"

int parse_expr(token_t *token, ST_stack *st_stack);

#endif
