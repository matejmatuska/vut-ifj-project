// htab.h -- IJC homework 2 2021
// License: None (Public domain)

#ifndef __HTAB_H__
#define __HTAB_H__

//This is the optimal value because it creates the largest possible hash table size and limits hashing collisions that cause unnecessary dependencies
#define MAX 65535

#include <string.h>     
#include <stdbool.h>    
#include <stdint.h>
#include <stdlib.h>
#include "scanner.h"
#include "error.h"

// symtable item type
typedef enum{
    HT_VAR,
    HT_FUNC,
    HT_STRING,
    HT_INT,
    HT_NIL
} htab_item_type;

// possible data types
typedef enum {
INTEGER,
NUMBER,
STRING,
NIL
}htab_datatype;

// Hash table structures:
struct htab;   
typedef struct htab htab_t;  

// DATA types
typedef const char *htab_key_t;       
typedef int htab_count_t;   
typedef bool htab_defined_t;           
typedef int num_of_params;

// item in hash table data
typedef struct  {  
    token_type      token_type; 
    htab_key_t      key;
    htab_count_t    count; 
    htab_item_type  item_type; 
    htab_datatype   datatype;  
    htab_defined_t  defined;
    num_of_params   params;
} htab_data_t;                

typedef struct htab_item_t {  
htab_data_t     data;
struct htab_item_t *next;
}htab_item_t;

struct htab
{
    size_t size;
    size_t arr_size;
    struct htab_item_t *element[];
};

// Symbol table types
htab_t *localsymtable;
htab_t *globalsymtable;

// hash function
size_t htab_hash_function(htab_key_t str);

// functions within hashtable:
htab_t *htab_init(size_t n);                        // constuctor of hasthable
htab_t *htab_move(size_t n, htab_t *from);          // move data to new hashtable
size_t htab_size(const htab_t * t);                 // number of items in hashtable
size_t htab_bucket_count( htab_t *t);               // size of array
htab_item_t * htab_find(htab_t *t, htab_key_t key); // find item in hashtable
htab_data_t * htab_lookup_add(htab_t *t, htab_key_t key); //insert item into table
bool htab_erase(htab_t * t, htab_key_t key);         // delete item
void htab_for_each(const htab_t * t, void (*f)(htab_data_t *data)); // make action for every item in hashtable
void htab_clear(htab_t * t);                        // delete all items
void htab_free(htab_t * t);                         // destructor of hashtable
bool isfunc(htab_t *t, htab_key_t key);                        //check if item is function
bool isvar(htab_t *t,htab_key_t key);                         //check if item is variable
#endif // __HTAB_H__
