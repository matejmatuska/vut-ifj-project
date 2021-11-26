/**
 * Stack used for bottom up expression parsing
 */
#ifndef SYMBOL_STACK_H
#define SYMBOL_STACK_H

#include <stddef.h>

#define DEFAULT_SIZE 20

// valid symbols
typedef enum {
    PLUS, // +
    MINUS, // -
    MULT, // *
    DIV, // "/"
    INT_DIV, // "//"
    LEQ, // <=
    LNE, // <
    GEQ, // >=
    GNE, // >
    EQ, // ==
    NEQ, // ~=
    LEN, // #
    CONCAT, // ..
    L_PAREN, // (
    R_PAREN, // )
    ID, // IDENTIFIER
    INT_LIT, // integer literal
    NUM_LIT, // number literal
    STR_LIT, // string literal
    NIL, // nil
    DOLLAR, // "$"
    HANDLE, // handle
    NON_TERMINAL // any non terminal symbol
} symbol_type_t;

typedef struct symbol {
    symbol_type_t type;
    struct symbol *next;
} symbol_t;

typedef struct symbol_stack {
    symbol_t *top;
} symbol_stack_t;

// initializes the stack, stack must no be NULL
void symbol_stack_init(symbol_stack_t *stack);

bool symbol_stack_push(symbol_stack_t *stack, symbol_type_t sym);

symbol_t *symbol_stack_top(symbol_stack_t *stack);

symbol_t *symbol_stack_top_terminal(symbol_stack_t *stack);

bool symbol_stack_pop(symbol_stack_t *stack);

/**
 * Pops all symbols before the handle from the stack
 * Only count symbols are valid
 *
 * @param count the lenght of the returned linked list
 * @return list of symbols before the handle or NULL if the handle is not present
 */
symbol_t *symbol_stack_top_to_handle(symbol_stack_t *stack, int *count);

/**
 * Inserts handle after the top terminal symbol
 */
bool symbol_stack_insert_handle(symbol_stack_t *stack);

/**
 * Free all memory used by stack
 */
void symbol_stack_free(symbol_stack_t *stack);

#endif
