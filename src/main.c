#include "hash_table.h"
#include <stdio.h>

int main(){
    //create a new hash table
    ht_hash_table* ht = ht_new();

    //insert key-value pairs
    ht_insert(ht, "name", "Ryan");
    ht_insert(ht, "city", "Tempe");

    char* name = ht_search(ht, "name");
    printf("Searching for name: %s\n", name);


    //delete a key-value pair
    ht_delete(ht, "city");

    ht_insert(ht, "job", "developer");
    ht_insert(ht, "degree", "computer science");

    // Insert more key-value pairs to trigger resizing
    ht_insert(ht, "key1", "value1");
    ht_insert(ht, "key2", "value2");
    ht_insert(ht, "key3", "value3");
    ht_insert(ht, "key4", "value4");
    ht_insert(ht, "key5", "value5");
    ht_insert(ht, "key6", "value6"); 

    char* keys[] = {"key7", "key8", "key9"};
    char* values[] = {"value7", "value8", "value9"};
    ht_batch_insert(ht, keys, values, sizeof(keys)/sizeof(keys[0]));
    

    ht_iterate(ht);
    ht_print_collision_stats(ht);
    ht_print_memory_usage(ht);

    ht_del_hash_table(ht);
    
    return 0;
}