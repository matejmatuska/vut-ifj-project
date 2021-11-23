#include "symtable.h"
#include "error.h"
//hash function djb2 (algorithm by dan bernstein)
unsigned long htab_hash_function(const char *str)
{
    unsigned long hash = 5381;
        int c;

        while ((c = *str++))
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
}

// initialization of table
htab_t *htab_init()
{
	htab_t *hashtable = malloc(sizeof(htab_t) + MAX * sizeof( htab_item_t *));
	if (hashtable == NULL)
	{
		return NULL;
	}   
	hashtable->size = 0;
	hashtable->arr_size = MAX;
	for (size_t i = 0; i < MAX; i++)
	{
		hashtable->element[i] = NULL;
	}
	return hashtable;
}

// create new item or update existing one 
htab_item_t *htab_add_item(htab_t *t, htab_key_t key)
{
	unsigned indx = htab_hash_function(key);
	size_t index = indx % t->arr_size;

	// if item already exists
	 htab_item_t *tmp = t->element[index];
	while (tmp != NULL)
	{
		if (!strcmp(tmp->key, key))
		{
			return tmp;
		}

			tmp = tmp->next;

	}

	// new item
	 htab_item_t *new = ( htab_item_t *)malloc(sizeof( htab_item_t));

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
// add data to item in hashtable
bool htab_add_data(htab_item_t *item,htab_item_type it,htab_datatype dt,bool def,int par)
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

//delete all items in symtable
void htab_clear(htab_t *t)
{
	 htab_item_t *item;
	 htab_item_t *ptr;
	for (size_t i = 0; i < t->arr_size; i++)
	{
		item = t->element[i];
		while (item != NULL)
		{
			ptr = item;
			item = item->next;
			//free((void *)ptr);
			free(ptr);
		}
		//initialization
		t->element[i] = NULL;
	}
	t->size = 0;
}
//delete element from symtable
bool htab_erase(htab_t * t, htab_key_t key)
{
	unsigned indx = htab_hash_function(key);
	size_t index = indx % t->arr_size;
	
	htab_item_t *tmp = t->element[index];
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
			struct htab_item_t *tmp_next = tmp->next->next;
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
// find specific item
htab_item_t *htab_find_in_table(htab_t *t, htab_key_t key)
{
	unsigned indx = htab_hash_function(key);
	size_t index = indx % t->arr_size;

	htab_item_t *item = t->element[index];

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


// make function *f for each item
void htab_for_each(const htab_t * t, void (*f)(htab_item_t *item))
{
	htab_item_t *item ;
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
void htab_free(htab_t * t)
{
    htab_clear(t);
    free(t);
}


// move one table to another
htab_t *htab_move(htab_t *from)
{
    htab_t *t = htab_init();
	
	for (unsigned i = 0; i < from->arr_size; i++)
	{
		htab_item_t *tmp = from->element[i];
		while(tmp != NULL)
		{
			if (htab_add_item(t, tmp->key) == NULL)
				return NULL;
			htab_find_in_table(t, tmp->key)->key = htab_find_in_table(from, tmp->key)->key;
			htab_find_in_table(t, tmp->key)->data.params = htab_find_in_table(from, tmp->key)->data.params;
			htab_find_in_table(t, tmp->key)->data.datatype = htab_find_in_table(from, tmp->key)->data.datatype;
			htab_find_in_table(t, tmp->key)->data.defined = htab_find_in_table(from, tmp->key)->data.defined;
			htab_find_in_table(t, tmp->key)->data.item_type = htab_find_in_table(from, tmp->key)->data.item_type;
			htab_find_in_table(t, tmp->key)->next = htab_find_in_table(from, tmp->key)->next;

			tmp = tmp->next;
		}
	}
	
	htab_clear(from);
	return t;
}
//return size of hashtable
size_t htab_size(const htab_t *t)
{
   return t->size;
}
//check if item is FUNC
bool isfunc(htab_t * t,htab_key_t key)
{
	if((htab_find_in_table(t,key)->data.item_type )== HT_FUNC)
	return true;
	else
	return false;

}
//check if item is VAR
bool isvar(htab_t * t, htab_key_t key)
{
	if((htab_find_in_table(t,key)->data.item_type )== HT_VAR)
	return true;
	else
	return false;

}

