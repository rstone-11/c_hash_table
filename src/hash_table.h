
typedef struct {
    char* key;
    char* value;
} ht_item;

typedef struct {
    int size;
    int count;
    ht_item** items;
    int base_size;
    int total_collisions;
    int max_collisions_insertion;
} ht_hash_table;

ht_hash_table* ht_new();
void ht_del_hash_table(ht_hash_table* ht);
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
int ht_delete(ht_hash_table* ht, const char* key);
void ht_iterate(ht_hash_table* ht);
double ht_get_avg_collisions(ht_hash_table* ht);
void ht_print_collision_stats(ht_hash_table* ht);