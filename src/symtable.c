#include "symtable.h"
#include "error.h"
/**
  * @desc hash function djb2 (algorithm by dan bernstein)
  * @param  const char *str - string we want to hash
  * @return unsigned long - hash number
*/
unsigned long sym_tab_hash_function(const char *str)
{
    unsigned long hash = 5381;
        int c;

        while ((c = *str++))
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
}

/**
  * @desc initialization of symbol table
  * @return sym_tab_t * - initialized symbol table
*/

sym_tab_t *sym_tab_init()
{
	sym_tab_t *hassym_table = malloc(sizeof(sym_tab_t) + MAX * sizeof( sym_tab_item_t *));
	if (hassym_table == NULL)
	{
		return NULL;
	}   
	hassym_table->size = 0;
	hassym_table->arr_size = MAX;
	for (size_t i = 0; i < MAX; i++)
	{
		hassym_table->element[i] = NULL;
	}
	return hassym_table;
}

/**
  * @desc add or update item to symbol table
  * @param  sym_tab_t *t - current symbol table
  * @param  sym_tab_key_t key - key we want to add
  * @return sym_tab_item_t - added item
*/
sym_tab_item_t *sym_tab_add_item(sym_tab_t *t, sym_tab_key_t key)
{
	unsigned indx = sym_tab_hash_function(key);
	size_t index = indx % t->arr_size;

	// if item already exists
	 sym_tab_item_t *tmp = t->element[index];
	while (tmp != NULL)
	{
		if (!strcmp(tmp->key, key))
		{
			return tmp;
		}

			tmp = tmp->next;

	}

	// new item
	 sym_tab_item_t *new = ( sym_tab_item_t *)malloc(sizeof( sym_tab_item_t));

	if (new == NULL)
	{
		return NULL;
	}

	new->key = NULL;
	new->key = malloc(sizeof(char) * (strlen(key) + 1));

	if (new->key == NULL)
	{
		free(new);
		return NULL;
	}

	strcpy((char *)new->key, key);
	new->data.datatype = NIL;
	new->data.defined = false;
	new->data.item_type = HT_OTHERS;
	new->data.params 	= 0;
	new->next = NULL;

	// add element at the beginning if the array was empty
	if (t->element[index] == NULL)
		t->element[index] = new;

	// add as last item
	else if (tmp->next == NULL)
		tmp->next = new;

	t->size++;
	return new;
}
/**
  * @desc add data to item in symbol table
  * @param sym_tab_item_t *item - specific item
  * @param  sym_tab_item_type it - item type
  * @param  sym_tab_datatype dt - data type
  * @param  bool def - defined
  * @param  int par - number of parameters
  * @return sym_tab_item_t - added item
*/
bool sym_tab_add_data(sym_tab_item_t *item,sym_tab_item_type it,sym_tab_datatype dt,bool def,int par)
{
	if(!item)
	{
		return false;
	}
	item->data.item_type = it;
	item->data.datatype = dt;
	item->data.defined = def;
	item->data.params = par;
	return true;
}

/**
  * @desc delete all items in symbol table
  * @param  sym_tab_t *t - current symbol table
  * @return nothing
*/
void sym_tab_clear(sym_tab_t *t)
{
	 sym_tab_item_t *item;
	 sym_tab_item_t *ptr;
	for (size_t i = 0; i < t->arr_size; i++)
	{
		item = t->element[i];
		while (item != NULL)
		{
			ptr = item;
			item = item->next;
			free((void *)ptr->key);
			free(ptr);
		}
		//initialization
		t->element[i] = NULL;
	}
	t->size = 0;
}
/**
  * @desc delete element from symbol table
  * @param  sym_tab_t *t - current symbol table
  * @param  sym_tab_key_t key - key we want to delete
  * @return bool - true if deletion was succesful
*/
bool sym_tab_erase(sym_tab_t * t, sym_tab_key_t key)
{
	unsigned indx = sym_tab_hash_function(key);
	size_t index = indx % t->arr_size;
	
	sym_tab_item_t *tmp = t->element[index];
	while(tmp != NULL)
	{
		// first element is a match witch key
		if (!strcmp(tmp->key, key))
		{
			t->element[index] = tmp->next;
			free(tmp);
			t->size--;
			return true;
		}
		
		// match with tmp-> next
		else if (tmp->next != NULL && !strcmp(tmp->next->key, key))
		{
			struct sym_tab_item_t *tmp_next = tmp->next->next;
			free(tmp->next);
			tmp->next = tmp_next;
			t->size--;
			return true;
		}
		
		// next element
		else
		{
			tmp = tmp->next;
		}
	}
	return false;
}
/**
  * @desc find item in symbol table
  * @param  sym_tab_t *t - current symbol table
  * @param  sym_tab_key_t key - key we want to find
  * @return sym_tab_item_t - found item
*/
sym_tab_item_t *sym_tab_find_in_table(sym_tab_t *t, sym_tab_key_t key)
{
	unsigned indx = sym_tab_hash_function(key);
	size_t index = indx % t->arr_size;

	sym_tab_item_t *item = t->element[index];

	while(item != NULL)
	{
		if(strcmp(item->key,key) == 0)
		{ 
			return item;
			break;
		}
		else
		{ 
			item = item->next;
		}
	}
	return NULL;
}


/**
  * @desc make action for every item in symbol table
  * @param  sym_tab_t *t - current symbol table
  * @param  void (*f)(sym_tab_item_t) item - function we want to call
  * @return nothing
*/
void sym_tab_for_each(const sym_tab_t * t, void (*f)(sym_tab_item_t *item))
{
	sym_tab_item_t *item ;
	for(unsigned i = 0; i < t->arr_size; i++)
	{ 
		item = t->element[i]; 
		while(item != NULL)
		{ 
			(*f)(item);
			item = item->next;
		}
	}
}

//free
void sym_tab_free(sym_tab_t * t)
{
    sym_tab_clear(t);
    free(t);
}


/**
  * @desc move one table to another
  * @param  sym_tab_t *from - symbol table we want to move
  * @return sym_tab_item_t - added item
*/
sym_tab_t *sym_tab_move(sym_tab_t *from)
{
    sym_tab_t *t = sym_tab_init();
	
	for (unsigned i = 0; i < from->arr_size; i++)
	{
		sym_tab_item_t *tmp = from->element[i];
		while(tmp != NULL)
		{
			if (sym_tab_add_item(t, tmp->key) == NULL)
				return NULL;
			sym_tab_find_in_table(t, tmp->key)->key = sym_tab_find_in_table(from, tmp->key)->key;
			sym_tab_find_in_table(t, tmp->key)->data.params = sym_tab_find_in_table(from, tmp->key)->data.params;
			sym_tab_find_in_table(t, tmp->key)->data.datatype = sym_tab_find_in_table(from, tmp->key)->data.datatype;
			sym_tab_find_in_table(t, tmp->key)->data.defined = sym_tab_find_in_table(from, tmp->key)->data.defined;
			sym_tab_find_in_table(t, tmp->key)->data.item_type = sym_tab_find_in_table(from, tmp->key)->data.item_type;
			sym_tab_find_in_table(t, tmp->key)->next = sym_tab_find_in_table(from, tmp->key)->next;

			tmp = tmp->next;
		}
	}
	
	sym_tab_clear(from);
	return t;
}
//return size of hassym_table
size_t sym_tab_size(const sym_tab_t *t)
{
   return t->size;
}
/**
  * @desc find out if item is function
  * @param  sym_tab_t *t - current symbol table
  * @param  sym_tab_key_t key - key we want to check
  * @bool  - true if it's function
*/
bool isfunc(sym_tab_t * t,sym_tab_key_t key)
{
	if((sym_tab_find_in_table(t,key)->data.item_type )== HT_FUNC)
	return true;
	else
	return false;

}
/**
  * @desc find out if item is a variable
  * @param  sym_tab_t *t - current symbol table
  * @param  sym_tab_key_t key - key we want to check
  * @bool  - true if it's variable
*/
bool isvar(sym_tab_t * t, sym_tab_key_t key)
{
	if((sym_tab_find_in_table(t,key)->data.item_type )== HT_VAR)
	return true;
	else
	return false;

}

