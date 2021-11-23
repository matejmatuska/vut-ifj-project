#include <stdio.h>
#include <stdlib.h>

#include "symtable.h"

typedef struct Hashstack
{
    htab_t *localtable;
    struct Hashstack *next;
}Hashstack;

htab_t *top_table(Hashstack*);
void push(struct Hashstack **);
void pop(Hashstack**);
htab_item_t *scope_search(Hashstack**,htab_key_t key);
void start(Hashstack*);
