#ifndef ST_STACK
#define ST_STACK

#include <stdio.h>
#include <stdlib.h>

#include "symtable.h"

// structure of stack of symbol tables
typedef struct ST_stack
{
    sym_tab_t *localtable;
    struct ST_stack *next;
} ST_stack;
// functions
ST_stack *init_ST_stack();                                    // initialize stack of symbol tables
sym_tab_t *top_table(ST_stack *);                             // current table of symbols
bool push(struct ST_stack **);                                // push another table of symbols
bool pop(ST_stack **);                                        // pop table of symbols
sym_tab_item_t *scope_search(ST_stack **, sym_tab_key_t key); // search for key in every table of symbols
bool free_ST_stack(ST_stack **);                              // freee stack of symbol tables
bool isfunc(ST_stack **scope, sym_tab_key_t key);             // check if item is function
bool isvar(ST_stack **scope, sym_tab_key_t key);              // check if item is variable
#endif