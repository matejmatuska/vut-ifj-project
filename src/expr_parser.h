/*
Implementace překladače imperativního jazyka IFJ21
Matej Matuška, xmatus36
*/
#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include "sym_stack.h"
#include "scanner.h"
#include "ST_stack.h"

/**
 * Runs the expression parser
 *
 * @param token the token handed from the main parser
 * @param st_stack symbol table stack
 * @param res_type resulting data type of the whole expression
 * @return error code
 */
int parse_expr(token_t *token, ST_stack *st_stack, data_type_t *res_type);

#endif
