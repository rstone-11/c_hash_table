#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "hash_table.h"
#include "prime.h"

#define HT_PRIME_1 151
#define HT_PRIME_2 163
#define HT_INITIAL_BASE_SIZE 10
static ht_item HT_DELETED_ITEM = {NULL, NULL};

/*
Creates a new key-value pair (ht_item) for the the hash table
*/
static ht_item* ht_new_item(const char* k, const char* v){
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}

/*
deletes an ht_item by freeing the memory allocated for the key-value
then the item itself
*/
static void ht_del_item(ht_item* i){
    free(i->key);
    free(i->value);
    free(i);
}

/*
hashes a string using a combination of a prime number and a modulo
iteratively multiplies each characters ascii value by the decreasing power of prime a
and summing the results to modulo m so it stays within range
*/
static int ht_hash(const char* s, const int a, const int m){
    long hash = 0;
    const int len_s = strlen(s);

    for(int i = 0; i < len_s; i++){
        hash += (long)pow(a, len_s - (i+1)) * s[i];
        hash = hash % m;
    }

    return (int)hash;
}

/*
combines two hash values to compute the final index in the hash table
the second hash helps in finding a new slot when collisions occur
*/
static int ht_get_hash(const char* s, const int num_buckets, const int attempt){
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);

    //printf("Hash A: %i\n", hash_a);
    //printf("Hash B: %i\n", hash_b);

    int index = (hash_a + (attempt * (hash_b+1))) % num_buckets;
    //printf("Hashing key \"%s\": Attempt %d -> Index %d\n", s, attempt, index);
    return (hash_a + (attempt * (hash_b+1))) % num_buckets;
}

/*
custom wrapper for malloc - allocates uninitialized memory that adds error checking
if there is a failure in allocating memory, the program terminates
*/
static void* xmalloc(size_t size){
    void* ptr = malloc(size);
    if(ptr == NULL){
        fprintf(stderr, "Error: Out of memory, failed to allocate %zu bytes.\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

/*
custom wrapper for calloc - allocates memorby initialized to zero that adds error checking
if there is an allocation failure, the program terminates
*/
static void* xcalloc(size_t num, size_t size){
    void* ptr = calloc(num, size);
    if(ptr == NULL){
        fprintf(stderr, "Error: Out of memory, failed to allocate %zu bytes.\n", num * size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

/*
initializes a new hash table on a given base size, with a prime number
allocates memory for a new hash table and initialize memory for items to zero
*/
static ht_hash_table* ht_new_sized(const int base_size){
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);

    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
    ht->total_collisions = 0;
    ht->max_collisions_insertion = 0;
    return ht;
}

/*
resizes the hash table up or down based on current load
rehashes all existing items and swaps new hash with the old one
size will be the first prime number greater than size passed in
*/
static void ht_resize(ht_hash_table* ht, const int base_size){
    if(base_size < HT_INITIAL_BASE_SIZE){
        return;
    }
    printf("Resizing from %d to %d\n", ht->base_size, base_size);
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for(int i = 0; i < ht->size; i++){
        ht_item* item = ht->items[i];
        if(item != NULL && item != &HT_DELETED_ITEM){
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;
    ht->total_collisions = new_ht->total_collisions;
    ht->max_collisions_insertion = new_ht->max_collisions_insertion;

    //need to keep track of size as its used to iterate through items to delete them
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);

}

/*
sets the size of the hash table to the first prime number double the base size
*/
static void ht_resize_up(ht_hash_table* ht){
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}

/*
sets the size of the hash table to the first prime number half the base size
*/
static void ht_resize_down(ht_hash_table* ht){
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}

/*
adds up the number of bytes the hash table takes up in memory
ht_hash_table struct + items pointer array + each ht_item
*/
static size_t ht_memory_usage(ht_hash_table* ht){

    size_t table_size = sizeof(ht_hash_table) + (ht->size * sizeof(ht_item*));
    size_t items_size = 0;

    for(int i = 0; i < ht->size; i++){
        ht_item* item = ht->items[i];
        if(item != NULL && item != &HT_DELETED_ITEM){
            items_size += sizeof(ht_item);
            items_size += strlen(item->key) + 1;
            items_size += strlen(item->value) + 1;
        }

    }
    return table_size + items_size;
}

/*
creates a new hash table with teh default initial size
*/
ht_hash_table* ht_new(){
    printf("Hash table starting at a base size of: %i\n", HT_INITIAL_BASE_SIZE);
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

/*
deletes each item in the hash table and then the hash table itself
*/
void ht_del_hash_table(ht_hash_table* ht){

    for(int i = 0; i < ht->size; i++){
        ht_item* item = ht->items[i];
        if(item != NULL){
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

/*
insert a key-value pair into the hash table
checks if more than 70% of table is filled, if so it resizes up
creates the item and finds the index through hashing
if key already exists updates the value
tracks number of collisions when attempting to insert item
*/
void ht_insert(ht_hash_table* ht, const char* key, const char* value){
    //printf("Inserting key \"%s\" with value \"%s\"\n", key, value);
    const int load = ht->count * 100 / ht->size;
    if(load > 70){
        printf("\n%i of %i buckets were used\nLoad factor exceeded, resizing up.\n\n", ht->count, ht->size);
        ht_resize_up(ht);
    }

    int collisions = 0;

    ht_item* item = ht_new_item(key, value);
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;
    while(cur_item != NULL){
        if(cur_item != &HT_DELETED_ITEM){
            if(strcmp(cur_item->key, key) == 0){
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
        collisions++;
        //printf("There was a collision\n");
    }
    //printf("Inserted key \"%s\" at index %d\n", key, index);
    ht->items[index] = item;
    ht->count++;
    ht->total_collisions += collisions;
    //printf("increased total collisions by %i to %i\n", collisions, ht->total_collisions);

    if(collisions > ht->max_collisions_insertion){
        ht->max_collisions_insertion = collisions;
    }
}

/*
inserts multiple key-value pairs into the hash table
takes in a hash table pointer, array of keys, array of values, and number of pairs to insert
*/
void ht_batch_insert(ht_hash_table* ht, char** keys, char** values, size_t count){

    for(size_t i = 0; i < count; i++){
        ht_insert(ht, keys[i], values[i]);
    }

}

/*
searches for a key in the hash table and returns the value, NULL if not found
does this by hashing the key and checking that index in the hash table
*/
char* ht_search(ht_hash_table* ht, const char* key){
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while(item != NULL){
        if(item != &HT_DELETED_ITEM){
            if(strcmp(item->key, key) == 0){
                return item->value;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    return NULL;
}

/*
removes a key-value pair from the hash table
if less than 10% of table is filled, it resizes-down
marks the slot as deleted with a const variable
if key doesn't exist nothing changes
returns 1 if an item was removed, 0 otherwise
*/
int ht_delete(ht_hash_table* ht, const char* key){

    const int load = ht->count * 100 / ht->size;
    if(load < 10){
        ht_resize_down(ht);
    }

    int item_deleted = 0;

    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while(item != NULL){
        
        if(item != &HT_DELETED_ITEM){
            if(strcmp(item->key, key) == 0){
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
                item_deleted = 1;
                break;
            }
        }

        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }

    if(item_deleted){
        ht->count--;
        return 1;
    }
    return 0;
}

/*
prints every key-value pair in the hash table
*/
void ht_iterate(ht_hash_table* ht){

    for(int i = 0; i < ht->size; i++){
        ht_item* item = ht->items[i];
        if(item != NULL &&item != &HT_DELETED_ITEM){
            printf("At bucket %i, %s : %s\n", i, item->key, item->value);
        }
    }

}

/*
gets the average number of collisions per insertion for the table as a decimal
*/
double ht_get_avg_collisions(ht_hash_table* ht){

    if(ht->count == 0){
        return 0;
    }
    //printf("total collisions: %i\n", ht->total_collisions);
    //printf("count: %i\n", ht->count);
    return (double)ht->total_collisions / ht->count;

}

/*
prints the collisions statistics for the hash table
*/
void ht_print_collision_stats(ht_hash_table* ht){

    printf("\nTable holds %i keys with a size of %d\n", ht->count, ht->size);
    printf("There have been %i collisions with an avg collision per insertion of %.2f\n", ht->total_collisions, ht_get_avg_collisions(ht));
    printf("The max collisions on a single insertion was %i\n", ht->max_collisions_insertion);

}

/*
prints the number of bytes the hash table takes up in memory
*/
void ht_print_memory_usage(ht_hash_table* ht){
    size_t size = ht_memory_usage(ht);
    printf("Size of the hash table is %zu bytes\n", size);
}