/*
Implementace překladače imperativního jazyka IFJ21
Matej Matuška, xmatus36
*/
/**
 * Stack used for bottom up expression parsing
 */
#ifndef SYMBOL_STACK_H
#define SYMBOL_STACK_H

#include <stddef.h>
#include "scanner.h"

// valid symbols
typedef enum {
    S_PLUS, // +
    S_MINUS, // -
    S_MULT, // *
    S_DIV, // "/"
    S_INT_DIV, // "//"
    S_LEQ, // <=
    S_LNE, // <
    S_GEQ, // >=
    S_GNE, // >
    S_EQ, // ==
    S_NEQ, // ~=
    S_LEN, // #
    S_CONCAT, // ..
    S_L_PAREN, // (
    S_R_PAREN, // )
    S_ID, // IDENTIFIER
    S_INT_LIT, // integer literal
    S_NUM_LIT, // number literal
    S_STR_LIT, // string literal
    S_NIL, // nil
    S_DOLLAR, // "$"
    S_HANDLE, // handle
    S_NON_TERMINAL // any non terminal symbol
} sym_type_t;

typedef enum {
    T_UNKNOWN,
    T_BOOL,
    T_INT,
    T_NUMBER,
    T_STRING,
    T_NIL
} data_type_t;

typedef struct symbol {
    sym_type_t type;
    data_type_t data_type;
    struct symbol *next;
} symbol_t;

// stack of symbols
typedef struct {
    symbol_t *top;
} sym_stack_t;

typedef struct {
    symbol_t *head;
    int len;
} sym_list_t;

/**
 * Initializes symbol_stack
 *
 * @param stack stack to be initialized
 */
void sym_stack_init(sym_stack_t *stack);

/**
 * Pushes symbol to top of the stack
 *
 * @param stack to push to
 * @param sym symbol to push
 */
bool sym_stack_push(sym_stack_t *stack, sym_type_t sym, data_type_t data_type);

/**
 * Returns symbol on top of stack
 * @param stack the stack
 */
symbol_t *sym_stack_top(sym_stack_t *stack);

/**
 * Returns top terminal symbol from the stack
 */
symbol_t *sym_stack_top_terminal(sym_stack_t *stack);

/**
 * Pops symbol off top of the stack
 */
bool sym_stack_pop(sym_stack_t *stack);

/**
 * Pops all symbols before the handle from the stack
 * Only count symbols are valid
 *
 * @param count the lenght of the returned linked list
 * @return list of symbols before the handle or NULL if the handle is not present
 */
symbol_t *sym_stack_top_to_handle(sym_stack_t *stack, int *count);

/**
 * Inserts handle symbol after the top terminal symbol
 */
bool sym_stack_insert_handle(sym_stack_t *stack);

/**
 * Free all memory used by stack
 */
void sym_stack_free(sym_stack_t *stack);

#endif
