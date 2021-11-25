#include "ST_stack.h"
#include "error.h"

/**
  * @desc acces to the top of ST_stack
  * @param  ST_stack *scope - current scope's visibility
*/
sym_tab_t *top_table(ST_stack *scope)
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
  * @param  ST_stack **scope - current scope's visibility
  * @return nothing
*/
bool push(struct ST_stack **scope)
{
    ST_stack *new = NULL;
    sym_tab_t *table = sym_tab_init();
    new = (ST_stack *)malloc(sizeof(ST_stack));
    // check for possible stack overflow
    if (!new)
    {
        return false;
    }
    
    if(!*scope)
    new->next = NULL;
    new->next = *scope;
    new->localtable = table;
    *scope = new;
    return true;
}

/**
  * @desc pop the top of ST_stack
  * @param  ST_stack *scope - current scope's visibility
  * @return nothing
*/
bool pop(ST_stack **scope)
{
    ST_stack *tmp;
    if (*scope == NULL)
    {
        return false;
    }
    tmp = *scope;
    *scope = (*scope)->next;
    sym_tab_free(tmp->localtable);
    free(tmp);
    return true;
}
/**
  * @desc search for item everywhere (local -> global)
  * @param  ST_stack *scope - current scope's visibility
  * @param  sym_tab_key_t key - key we want to find 
  * @return nothing
*/
sym_tab_item_t *scope_search(ST_stack** scope ,sym_tab_key_t key)
{
     ST_stack *top = *scope;
     if(top == NULL)
     {
      printf("\nStack is Empty!!!\n");
      return NULL;
     }
   else{
      ST_stack *temp = top;
      while(temp != NULL){
     sym_tab_item_t*tmp = NULL;
	 if((tmp = sym_tab_find_in_table(temp->localtable,key)))
     return tmp;
	 temp = temp->next;
      }
      return NULL;
   }
}
