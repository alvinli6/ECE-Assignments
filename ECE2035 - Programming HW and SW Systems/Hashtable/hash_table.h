/****************************************************************************
 * Include guards
 *
 * By using a preprecessor include guard like this one (along with the #endif
 * at the bottom of the file), we can guarantee that the public interface for
 * this module is only included once in each compilation unit. This prevents
 * transitive dependencies from mistakenly including headers many times, and
 * can also prevent infinite #include loops due to circular dependencies.
 ***************************************************************************/
#ifndef HASHTABLE_H
#define HASHTABLE_H

/****************************************************************************
 * Forward Declarations
 *
 * These declarations are for interface types that are private to the module,
 * but are needed for external interfaces. Without a definition, the compiler
 * (and therefore the user) does not have access to information about the
 * member variables, and so the members cannot be used directly from modules
 * that include this header. However, we do know that these structures are
 * valid, and we can use pointers to them. This technique allows hiding the
 * implementation details of the hash table module behind a clean public
 * interface.
 ***************************************************************************/
 /**
  * This defines a type that is a pointer to a function which takes
  * an unsigned int argument and returns an unsigned int value.
  * The name of the type is "HashFunction".
  */
typedef unsigned int (*HashFunction)(unsigned int key);

/**
 * This defines a type that is a _HashTable struct. The definition for
 * _HashTable is implemented in hash_table.c.
 *
 * In other words, "HashTable" is an alternative name for "struct _HashTable".
 * "HashTable" can be used to create a new struct variable.
 */
typedef struct _HashTable HashTable;

/**
 * This defines a type that is a _HashTableEntry struct. The definition for
 * _HashTableEntry is implemented in hash_table.c.
 *
 * In other words, "HashTableEntry" is an alternative name for "struct _HashTableEntry".
 * "HashTableEntry" can be used to create a new struct variable.
 */
typedef struct _HashTableEntry HashTableEntry;

/**
 * createHashTable
 *
 * Creates a hash table by allocating memory for it on the heap. Initialize num_buckets
 * and hash based on function arguments. Allocate memory for buckets as an array of
 * pointers to HashTableEntry objects based on the number of buckets available.
 * Each bucket contains a singly linked list, whose nodes are HashTableEntry objects.
 *
 * @param myHashFunc The pointer to the custom hash function.
 * @param numBuckets The number of buckets available in the hash table.
 * @return a pointer to the new hash table
 */
HashTable* createHashTable(HashFunction myHashFunc, unsigned int numBuckets);

/**
 * destroyHashTable
 *
 * Destroy the hash table. The nodes (HashTableEntry objects) of singly linked
 * list, the values stored on the linked list, the buckets, and the hashtable
 * itself are freed from the heap. In other words, free all the allocated memory
 * on heap that is associated with heap, including the values that users store in
 * the hash table.
 *
 * @param myHashTable The pointer to the hash table.
 *
 */
void destroyHashTable(HashTable* myHashTable);

/**
 * insertItem
 *
 * Insert the value into the hash table based on the key.
 * In other words, create a new hash table entry and add it to a specific bucket.
 *
 * @param myHashTable The pointer to the hash table.
 * @param key The key that corresponds to the value.
 * @param value The value to be stored in the hash table.
 * @return old value if it is overwritten, or NULL if not replaced
 */
void* insertItem(HashTable* myHashTable, unsigned int key, void* value);

/**
 * getItem
 *
 * Get the value that corresponds to the key in the hash table.
 *
 * @param myHashTable The pointer to the hash table.
 * @param key The key that corresponds to the item.
 * @return the value corresponding to the key, or NULL if the key is not present
 */
void* getItem(HashTable* myHashTable, unsigned int key);

/**
 * removeItem
 *
 * Remove the item in hash table based on the key and return the value stored in it.
 * In other words, return the value and free the hash table entry from heap.
 *
 * @param myHashTable The pointer to the hash table.
 * @param key The key that corresponds to the item.
 * @return the pointer of the value corresponding to the key, or NULL if the key is not present
 */
void* removeItem(HashTable* myHashTable, unsigned int key);

/**
 * deleteItem
 *
 * Delete the item in the hash table based on the key. In other words, free the
 * value stored in the hash table entry and the hash table entry itself from
 * the heap.
 *
 * @param myHashTable The pointer to the hash table.
 * @param key The key that corresponds to the item.
 *
 */
void deleteItem(HashTable* myHashTable, unsigned int key);

#endif
