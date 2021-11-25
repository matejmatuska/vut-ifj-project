#ifndef ST_STACK
#define ST_STACK

#include <stdio.h>
#include <stdlib.h>

#include "symtable.h"

//structure of stack
typedef struct ST_stack
{
    sym_tab_t *localtable;
    struct ST_stack *next;
}ST_stack;
//functions
sym_tab_t *top_table(ST_stack*);  //current table of symbols
bool push(struct ST_stack **);    //push another table of symbols
bool pop(ST_stack**);              // pop table of symbols
sym_tab_item_t *scope_search(ST_stack**,sym_tab_key_t key); // search for key in every table of symbols
#endif