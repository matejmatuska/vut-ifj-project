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
htab_t *htab_init(size_t n)
{
	htab_t *hashtable = malloc(sizeof(htab_t) + n * sizeof( htab_item_t *));
	if (hashtable == NULL)
	{
		return NULL;
	}   
	hashtable->size = 0;
	hashtable->arr_size = n;
	for (size_t i = 0; i < n; i++)
	{
		hashtable->element[i] = NULL;
	}
	return hashtable;
}

// add new item or update existing one 
htab_data_t *htab_lookup_add(htab_t *t, htab_key_t key)
{
	unsigned indx = htab_hash_function(key);
	size_t index = indx % t->arr_size;

	// if item already exists
	 htab_item_t *tmp = t->element[index];
	while (tmp != NULL)
	{
		if (!strcmp(tmp->data.key, key))
		{
			tmp->data.count++;
			htab_data_t *data = &(tmp->data);
			return data;
		}

		if (tmp->next != NULL)
			tmp = tmp->next;

		else
			break;
	}

	// new item
	 htab_item_t *new = ( htab_item_t *)malloc(sizeof( htab_item_t));

	if (new == NULL)
	{
		fprintf(stderr, "Allocation unsuccessful\n");
		return NULL;
	}

	new->data.key = NULL;
	new->data.key = malloc(sizeof(char) * (strlen(key) + 1));

	if (new->data.key == NULL)
	{
		free(new);
		return NULL;
	}

	strcpy((char *)new->data.key, key);
	new->data.count = 1;
	new->data.datatype = NIL;
	new->data.defined = false;
	new->data.item_type = HT_NIL;
	new->data.token_type = TOKEN_TYPE_EOF;
	new->data.params 	= 0;
	new->next = NULL;

	// add element at the beginning if the array was empty
	if (t->element[index] == NULL)
		t->element[index] = new;

	// add as last item
	else if (tmp->next == NULL)
		tmp->next = new;

	t->size++;
	htab_data_t *newdata = &(new->data);
	return newdata;
}
// size of array
size_t htab_bucket_count( htab_t *t)
{
    return t->arr_size;
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
		if (!strcmp(tmp->data.key, key))
		{
			t->element[index] = tmp->next;
			free(tmp);
			t->size--;
			return true;
		}
		
		// match with tmp-> next
		else if (tmp->next != NULL && !strcmp(tmp->next->data.key, key))
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
htab_item_t *htab_find(htab_t *t, htab_key_t key)
{
	unsigned indx = htab_hash_function(key);
	size_t index = indx % t->arr_size;

	htab_item_t *item = t->element[index];

	while(item != NULL)
	{
		if(strcmp(item->data.key,key) == 0)
		{ 
			htab_item_t *data =item;
			data->data = item->data;
			return data;
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
void htab_for_each(const htab_t * t, void (*f)(htab_data_t *data))
{
	htab_item_t *item ;
	for(unsigned i = 0; i < t->arr_size; i++)
	{ 
		item = t->element[i]; 
		while(item != NULL)
		{ 
			htab_data_t *data = &item->data;
			(*f)(data);
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
htab_t *htab_move(size_t n, htab_t *from)
{
    htab_t *t = htab_init(n);
	
	for (unsigned i = 0; i < from->arr_size; i++)
	{
		htab_item_t *tmp = from->element[i];
		while(tmp != NULL)
		{
			if (htab_lookup_add(t, tmp->data.key) == NULL)
				return NULL;

			htab_find(t, tmp->data.key)->data.key = htab_find(from, tmp->data.key)->data.key;
			htab_find(t, tmp->data.key)->data.count = htab_find(from, tmp->data.key)->data.count;
			htab_find(t, tmp->data.key)->data.params = htab_find(from, tmp->data.key)->data.params;
			htab_find(t, tmp->data.key)->data.token_type = htab_find(from, tmp->data.key)->data.token_type;
			htab_find(t, tmp->data.key)->data.datatype = htab_find(from, tmp->data.key)->data.datatype;
			htab_find(t, tmp->data.key)->data.defined = htab_find(from, tmp->data.key)->data.defined;
			htab_find(t, tmp->data.key)->data.item_type = htab_find(from, tmp->data.key)->data.item_type;

			tmp = tmp->next;
		}
	}
	
	htab_clear(from);
	return t;
}
size_t htab_size(const htab_t *t)
{
   return t->size;
}
bool isfunc(htab_t * t,htab_key_t key)
{
	if((htab_find(t,key)->data.item_type )== HT_FUNC)
	return true;
	else
	return false;

}
bool isvar(htab_t * t, htab_key_t key)
{
	if((htab_find(t,key)->data.item_type )== HT_VAR)
	return true;
	else
	return false;

}

