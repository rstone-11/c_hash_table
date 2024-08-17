# C Hash Table

This project is an implementation of a hash table in C. The hash table uses open addressing with double hashing for collision resolution. The goal of this project is to understand and implement the core functionalities of a hash table, including insertion, deletion, searching, resizing, while also tracking statistics about collisions and memory usage.

## Breakdown of the Hash Table

### What is a Hash Table?
A hash table is a data structure that allows you to store and retrieve data efficiently using keys. It uses a hash function to map keys to indices in an array, called buckets.

### Data Structures Used

- **`ht_item` struct**: Defines an item in the hash table, which stores a string key-value pair.
- **`ht_hash_table` struct**: Represents the hash table itself. It contains:
  - `size`: Number of buckets in the table.
  - `count`: Number of `ht_item` elements currently in the table.
  - `items`: An array of `ht_item` pointers. Each bucket is either `NULL`, a pointer to a valid `ht_item`, or a pointer to a special 'deleted' item.
  - `base_size`: The initial size of the hash table.

### Hash Function

The hash function computes a hash value for a given string by converting each character in the string to its numerical value. It uses a base prime number raised to the position of the character in the string and then takes the modulo of the number of buckets in the table to ensure the value fits within the bounds.

### Collision Resolution: Double Hashing

If the first hash leads to a collision, a second hash function is used to calculate a new index. The `attempt` parameter, which is incremented iteratively, determines how many times we've tried to insert or find the item. For each subsequent attempt, the index it hashes to is changed to reduce clustering.

### Insertion

To insert a new item into the hash table:
1. Hash the item with `attempt` set to 0.
2. Check if the bucket is already occupied:
   - If the key already exists, update the item.
   - If the bucket is occupied by another key, rehash with an incrementing `attempt` value until an empty bucket is found.

If the load factor exceeds 70%, the table is resized up (doubled in size). It resizes to the first prime number double the current size. 

### Deletion

Using open addressing makes deletion more complex:
- Directly removing an item could break a collision chain, making subsequent items in the chain inaccessible.
- A collision chain is a sequence of items that were inserted into the table at indicies that originally hashed to the same value, causing them to be place in subsequent spots.
- Instead of removing an item, we free the item and mark the bucket as deleted. This ensures that the search operation can continue past deleted items.

If the load factor drops below 10%, the table is resized down (halved in size). It resizes down to the first prime number greater than or equal to half the current size. 

### Searching

To search for an item:
1. Hash the key starting with `attempt` 0.
2. Increment `attempt` until the key is found or a `NULL` bucket is encountered (indicating the key does not exist).

## Features

- **Open Addressing with Double Hashing:** Handles collisions by probing alternative slots using double hashing.
- **Dynamic Resizing:** Automatically resizes the table when the load factor exceeds or drops below certain thresholds.
- **Collision Statistics:** Tracks the number of collisions during insertion and reports the maximum and average collisions.
- **Memory Usage Reporting:** Calculates and reports the total memory used by the hash table and each key-value pair.

## Design Choices

- **Open Addressing:** Stores all elements directly in the hash table. When a collision occurs, it probes for the next available slot.
- **Double Hashing:** Uses a secondary hash function to calculate a new index in case of a collision, reducing clustering.
- **Dynamic Resizing:** The hash table resizes based on the load factor. It doubles in size when the load factor exceeds 70% and halves when it drops below 10%.

## Usage

### Compiling the Program

In the root directory, run the following command:

```bash
gcc src/main.c src/hash_table.c src/prime.c -o a.out
```

Run the program:
```bash
./a.out
```

### Acknowledgements
Based on the outline found at: https://github.com/jamesroutley/write-a-hash-table