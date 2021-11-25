// sym_tab.h -- IJC homework 2 2021
// License: None (Public domain)

#ifndef SYMTABLE_H
#define SYMTABLE_H
//This is the optimal value because it creates the largest possible hash table size and limits hashing collisions
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
    HT_OTHERS
} sym_tab_item_type;

// possible data types
typedef enum {
INTEGER,
NUMBER,
STRING,
NIL
}sym_tab_datatype;

// Hash table structures:
struct sym_tab;   
typedef struct sym_tab sym_tab_t;  

// DATA types
typedef const char *sym_tab_key_t;       
typedef int sym_tab_count_t;   
typedef bool sym_tab_defined_t;   
typedef bool sym_tab_declared_t;        
typedef int num_of_params;

//linked list of data types
typedef struct datatypes_list {
    sym_tab_datatype datatype;
    struct datatypes_list *next;
 }datatypes_list;


typedef struct datatypes_list *data_type; 

// data of item in table
typedef struct  {   
    sym_tab_item_type  item_type; 
    data_type     return_data_types;
    data_type     param_data_types;
    sym_tab_defined_t  defined;
    sym_tab_declared_t declared;
    num_of_params   params;
} sym_tab_data_t;                

//structure of item in table
typedef struct sym_tab_item_t {  
sym_tab_key_t          key;
sym_tab_data_t         data;
struct sym_tab_item_t *next;
}sym_tab_item_t;

//structure of table
struct sym_tab
{
    size_t size;
    size_t arr_size;
    struct sym_tab_item_t *element[];
};

// hash function
size_t sym_tab_hash_function(sym_tab_key_t str);

// functions within sym_table:
sym_tab_t *sym_tab_init();                                // constuctor of hasthable
sym_tab_t *sym_tab_move(sym_tab_t *from);                    // move data to new hassym_table
size_t sym_tab_size(const sym_tab_t * t);                 // number of items in hassym_table
sym_tab_item_t * sym_tab_find_in_table(sym_tab_t *t, sym_tab_key_t key); // find item in hassym_table
sym_tab_item_t * sym_tab_add_item(sym_tab_t *t, sym_tab_key_t key);
bool sym_tab_add_data_function(sym_tab_item_t *item,data_type return_data_types,data_type param_data_types,sym_tab_declared_t dec,sym_tab_defined_t def,int par);
bool sym_tab_add_data_var(sym_tab_item_t *item,data_type param_data_types,sym_tab_declared_t dec,sym_tab_defined_t def);
bool sym_tab_erase(sym_tab_t * t, sym_tab_key_t key);         // delete item
void sym_tab_for_each(const sym_tab_t * t, void (*f)(sym_tab_item_t *item)); // make action for every item in hassym_table
void sym_tab_clear(sym_tab_t * t);                        // delete all items
void sym_tab_free(sym_tab_t * t);                         // destructor of hassym_table
bool isfunc(sym_tab_t *t, sym_tab_key_t key);                        //check if item is function
bool isvar(sym_tab_t *t,sym_tab_key_t key);   
data_type create_data_type(sym_tab_datatype first);  
data_type add_data_type(data_type first, sym_tab_datatype value);
void delete_data_types(data_type first);
#endif // __sym_tab_H__
