// sym_tab.h -- IJC homework 2 2021
// License: None (Public domain)

#ifndef SYMTABLE_H
#define SYMTABLE_H
// This is the optimal value because it creates the largest possible hash table size and limits hashing collisions
#define MAX 5

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "scanner.h"
#include "error.h"

// symtable item type
typedef enum
{
    HT_VAR,
    HT_FUNC,
    HT_OTHERS
} sym_tab_item_type;

// possible data types
typedef enum
{
    INTEGER,
    NUMBER,
    STRING,
    NIL
} sym_tab_datatype;

// Hash table structures:
struct sym_tab;
typedef struct sym_tab sym_tab_t;

// DATA types
typedef const char *sym_tab_key_t;
typedef int sym_tab_count_t;
typedef bool sym_tab_defined_t;
typedef bool sym_tab_declared_t;
typedef int num_of_params;
typedef int num_of_returns;

// linked list of data types
typedef struct datatypes_list
{
    sym_tab_datatype datatype;
    struct datatypes_list *next;
} datatypes_list;

// linked list structure
typedef struct datatypes_list *data_type;

// linked list of dynamic string + data types
typedef struct name_and_datatype
{
    sym_tab_datatype datatype;
    dynamic_string_t *string;
    struct name_and_datatype *next;
} name_and_datatype;

// linked list structure
typedef struct name_and_datatype *name_and_data;

// data of item in table
typedef struct
{
    sym_tab_item_type item_type; // type of item
    data_type return_data_types; // return data types of functions and variables
    data_type param_data_types;  // data types of parameters
    sym_tab_defined_t defined;   // bool defined
    sym_tab_declared_t declared; // bool declared
    num_of_params params;        // number of parameters
    num_of_returns returns;      // number of returns
} sym_tab_data_t;

// structure of item in table
typedef struct sym_tab_item_t
{
    sym_tab_key_t key;
    sym_tab_data_t data;
    struct sym_tab_item_t *next;
} sym_tab_item_t;

// structure of table
struct sym_tab
{
    size_t size;
    size_t arr_size;
    struct sym_tab_item_t *element[];
};

// hash function
size_t sym_tab_hash_function(sym_tab_key_t str);

// functions within sym_table:
sym_tab_t *sym_tab_init();                                              // constuctor of hasthable
sym_tab_t *sym_tab_move(sym_tab_t *from);                               // move data to new hassym_table
size_t sym_tab_size(const sym_tab_t *t);                                // number of items in hassym_table
sym_tab_item_t *sym_tab_find_in_table(sym_tab_t *t, sym_tab_key_t key); // find item in hassym_table
sym_tab_item_t *sym_tab_add_item(sym_tab_t *t, sym_tab_key_t key);      // create new item to add to symbol table
// add data of function
bool sym_tab_add_data_function(sym_tab_item_t *item, data_type return_data_types, data_type param_data_types, sym_tab_declared_t dec, sym_tab_defined_t def, int par,int ret);
// add data of variable
bool sym_tab_add_data_var(sym_tab_item_t *item, data_type return_data_types, sym_tab_declared_t dec, sym_tab_defined_t def);
bool sym_tab_erase(sym_tab_t *t, sym_tab_key_t key);                           // delete item
void sym_tab_for_each(const sym_tab_t *t, void (*f)(sym_tab_item_t *item));    // make action for every item in symbol table
void sym_tab_clear(sym_tab_t *t);                                              // delete all items
void sym_tab_free(sym_tab_t *t);                                               // destructor of symbol table
data_type create_data_type(sym_tab_datatype first);                            // [linked list function] - first value of list of data types
data_type add_data_type(data_type first, sym_tab_datatype value);              // [linked list function] - add more values to list of data types
void delete_data_types(data_type *first);                                      // [linked list function] - delete all values from linked list of data types
name_and_data create_name_data(sym_tab_datatype data, dynamic_string_t *name); // [linked list function 2] - first value of list of data types and names
name_and_data add_name_data(name_and_data first, sym_tab_datatype data);       // [linked list function 2] - add more values to list of data types and names
void delete_data_name(name_and_data *first);                                   // [linked list function 2] - delete all values from linked list of data types and names
#endif