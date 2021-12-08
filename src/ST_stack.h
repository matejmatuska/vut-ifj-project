#ifndef ST_STACK
#define ST_STACK

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "symtable.h"

// structure of stack of symbol tables
typedef struct ST_stack
{
    size_t level; // current nesting level beggining at 0
    size_t uid; // unique identifier for symtable
    struct st_stack_item *top;
} ST_stack;

typedef struct st_stack_item
{
    sym_tab_t *localtable;
    struct st_stack_item *next;
} st_stack_item_t;

// functions
void init_ST_stack(ST_stack *stack);                                    // initialize stack of symbol tables

// returns number of items on stack (current nesting level) starting from 0
size_t st_stack_level(ST_stack *stack);

sym_tab_t *top_table(ST_stack *stack);                             // current table of symbols

bool push(ST_stack *stack);                                // push another table of symbols

bool pop(ST_stack *stack);                                        // pop table of symbols

sym_tab_item_t *scope_search(ST_stack *stack, sym_tab_key_t key, size_t *uid, size_t *level);

bool free_ST_stack(ST_stack *stack);                              // freee stack of symbol tables

bool isfunc(ST_stack *stack, sym_tab_key_t key);             // check if item is function

bool isvar(ST_stack *stack, sym_tab_key_t key);              // check if item is variable
#endif
