#include "hashstack.h"
#include "error.h"


/**
  * @desc acces to the top of Hashstack
  * @param  Hashstack *scope - current scope's visibility
  * @return local symtable at the top of stack
*/
htab_t *top_table(Hashstack *scope)
{
    if (scope != NULL)
    {
        return scope->localtable;
    }
    else
        return NULL;
}
/**
  * @desc push another local table into the stack
  * @param  Hashstack **scope - current scope's visibility
  * @param  htab_t *table - table to be pushed
  * @return nothing
*/
void push(struct Hashstack **scope)
{
    Hashstack *new = NULL;
    htab_t *table = htab_init(MAX);
    new = (Hashstack *)malloc(sizeof(Hashstack));
    // check for possible stack overflow
    if (!new)
    {
        exit(1);
    }
    
    if(!*scope)
    new->next = NULL;
    new->next = *scope;
    new->localtable = table;
    *scope = new;
}

/**
  * @desc pop the top of Hashstack
  * @param  Hashstack *scope - current scope's visibility
  * @return nothing
*/
void pop(Hashstack **scope)
{
    Hashstack *tmp;
    if (*scope == NULL)
    {
        exit(1);
    }
    tmp = *scope;
    *scope = (*scope)->next;
    htab_free(tmp->localtable);
    free(tmp);
}

htab_item_t *scope_search(Hashstack** scope ,htab_key_t key)
{
     Hashstack *top = *scope;
     if(top == NULL)
     {
      printf("\nStack is Empty!!!\n");
      return NULL;
     }
   else{
      Hashstack *temp = top;
      while(temp != NULL){
     htab_item_t*tmp = NULL;
	 if((tmp = htab_find_in_table(temp->localtable,key)))
     return tmp;
	 temp = temp->next;
      }
      return NULL;
   }
}

void start(Hashstack *scope)
{
    push(&scope);
}