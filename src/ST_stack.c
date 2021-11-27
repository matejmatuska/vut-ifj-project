#include "ST_stack.h"
#include "error.h"

/**
  * Acces to the top of ST_stack
  * @param  ST_stack *scope -> current scope's visibility
  * @return top symbol table
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
  * Push another local table into the stack
  * @param  ST_stack **scope - current scope's visibility
  * @return true if success
*/
bool push(struct ST_stack **scope)
{
    ST_stack *new = NULL;
    sym_tab_t *table = sym_tab_init();
    new = (ST_stack *)malloc(sizeof(ST_stack));
    // check if stack was allocated correctly
    if (!new)
    {
        return false;
    }
    // if we are pushing first symbol table (aka global)
    new->next = *scope;
    new->localtable = table;
    *scope = new;
    return true;
}

/**
  * Pop the top of ST_stack
  * @param  ST_stack *scope - current scope's visibility
  * @return bool if success
*/
bool pop(ST_stack **scope)
{
    ST_stack *tmp;
    // check if scope is empty
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
  * Search for item everywhere (local -> global)
  * @param  ST_stack **scope - current scope's visibility
  * @param  sym_tab_key_t key - key we want to find
  * @return found item or NULL if item doesn't exist
*/
sym_tab_item_t *scope_search(ST_stack **scope, sym_tab_key_t key)
{
    ST_stack *top = *scope;
    // if symbol table is empty
    if (top == NULL)
    {
        return NULL;
    }
    else
    {
        ST_stack *temp = top;
        while (temp != NULL)
        {
            sym_tab_item_t *tmp = NULL;
            if ((tmp = sym_tab_find_in_table(temp->localtable, key)))
                return tmp;
            temp = temp->next;
        }
        return NULL;
    }
}

/**
  * Initialize stack
  * @return initialized stack
*/
ST_stack *init_ST_stack()
{
    ST_stack *scope = NULL;
    return scope;
}

/**
  * Free all remaining stacks
  * @param ST_stack **scope -> current scope's visibilty
  * @return true if succesfuly freed
*/
bool free_ST_stack(ST_stack **scope)
{
    ST_stack *top = *scope;
    // check for empty symbol table
    if (top == NULL)
    {
        return false;
    }
    else
    {
        // loop through symtables
        while (top != NULL)
        {
            pop(&top);
        }
        return true;
    }
    return false;
}
/**
  * Find out if item is function
  * @param  sym_tab_t *t - current symbol table
  * @param  sym_tab_key_t key - key we want to check
  * @return true if it's function
*/
bool isfunc(ST_stack **scope, sym_tab_key_t key)
{
    if (scope_search(scope, key) == NULL)
        return false;
    else
    {
        if (scope_search(scope, key)->data.item_type == HT_FUNC)
            return true;
        else
            return false;
    }
}
/**
  * Find out if item is a variable
  * @param  sym_tab_t *t - current symbol table
  * @param  sym_tab_key_t key - key we want to check
  * @bool  - true if it's variable
*/
bool isvar(ST_stack **scope, sym_tab_key_t key)
{

    if (scope_search(scope, key) == NULL)
        return false;
    else
    {
        if (scope_search(scope, key)->data.item_type == HT_VAR)
            return true;
        else
            return false;
    }
}
