/*
 Student Name: Alvin Li
 Date: 11/3/2019

=======================
ECE 2035 Project 2-1:
=======================
This file provides definition for the structs and functions declared in the
header file. It also contains helper functions that are not accessible from
outside of the file.

FOR FULL CREDIT, BE SURE TO TRY MULTIPLE TEST CASES and DOCUMENT YOUR CODE.

===================================
Naming conventions in this file:
===================================
1. All struct names use camel case where the first letter is capitalized.
  e.g. "HashTable", or "HashTableEntry"

2. Variable names with a preceding underscore "_" will not be called directly.
  e.g. "_HashTable", "_HashTableEntry"

  Recall that in C, we have to type "struct" together with the name of the struct
  in order to initialize a new variable. To avoid this, in hash_table.h
  we use typedef to provide new "nicknames" for "struct _HashTable" and
  "struct _HashTableEntry". As a result, we can create new struct variables
  by just using:
    - "HashTable myNewTable;"
     or
    - "HashTableEntry myNewHashTableEntry;"

  The preceding underscore "_" simply provides a distinction between the names
  of the actual struct defition and the "nicknames" that we use to initialize
  new structs.
  [See Hidden Definitions section for more information.]

3. Functions, their local variables and arguments are named with camel case, where
  the first letter is lower-case.
  e.g. "createHashTable" is a function. One of its arguments is "numBuckets".
       It also has a local variable called "newTable".

4. The name of a struct member is divided by using underscores "_". This serves
  as a distinction between function local variables and struct members.
  e.g. "num_buckets" is a member of "HashTable".

*/

/****************************************************************************
* Include the Public Interface
*
* By including the public interface at the top of the file, the compiler can
* enforce that the function declarations in the the header are not in
* conflict with the definitions in the file. This is not a guarantee of
* correctness, but it is better than nothing!
***************************************************************************/
#include "hash_table.h"


/****************************************************************************
* Include other private dependencies
*
* These other modules are used in the implementation of the hash table module,
* but are not required by users of the hash table.
***************************************************************************/
#include <stdlib.h>   // For malloc and free
#include <stdio.h>    // For printf


/****************************************************************************
* Hidden Definitions
*
* These definitions are not available outside of this file. However, because
* the are forward declared in hash_table.h, the type names are
* available everywhere and user code can hold pointers to these structs.
***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable {
  /** The array of pointers to the head of a singly linked list, whose nodes
      are HashTableEntry objects */
  HashTableEntry** buckets;

  /** The hash function pointer */
  HashFunction hash;

  /** The number of buckets in the hash table */
  unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry {
  /** The key for the hash table entry */
  unsigned int key;

  /** The value associated with this hash table entry */
  void* value;

  /**
  * A pointer pointing to the next hash table entry
  * NULL means there is no next entry (i.e. this is the tail)
  */
  HashTableEntry* next;
};


/****************************************************************************
* Private Functions
*
* These functions are not available outside of this file, since they are not
* declared in hash_table.h.
***************************************************************************/
/**
* createHashTableEntry
*
* Helper function that creates a hash table entry by allocating memory for it on
* the heap. It initializes the entry with key and value, initialize pointer to
* the next entry as NULL, and return the pointer to this hash table entry.
*
* @param key The key corresponds to the hash table entry
* @param value The value stored in the hash table entry
* @return The pointer to the hash table entry
*/
static HashTableEntry* createHashTableEntry(unsigned int key, void* value) {

   HashTableEntry* newEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));      //create pointer to new entry with allocated size
   newEntry->key = key;         //new entry will have its structural elements set equal to arguments passed into function
   newEntry->value = value;
   newEntry->next = NULL;       //since it will be last entry, the struct's next pointer will point to null

   return newEntry;     //function returns pointer to entry   
}

/**
* findItem
*
* Helper function that checks whether there exists the hash table entry that
* contains a specific key.
*
* @param hashTable The pointer to the hash table.
* @param key The key corresponds to the hash table entry
* @return The pointer to the hash table entry, or NULL if key does not exist
*/
static HashTableEntry* findItem(HashTable* hashTable, unsigned int key) {
    int index = hashTable->hash(key);   //find bucket index of the hastable that this item will be in
    HashTableEntry* node = hashTable->buckets[index];   //create pointer at an entry at the beginning of linkedlist for found bucket

    while(node)     //while there is an entry
    {
        if(node->key == key)    //if the entry's key is the key that was passed into function, that entry is the item to be found
        {
          return node;
        }
        node = node->next;      //if the entry's key does not match the key passed into function, just update entry pointer and go to next entry
    } 
    return node;        //return node at the end after entry found
}

/****************************************************************************
* Public Interface Functions
*
* These functions implement the public interface as specified in the header
* file, and make use of the private functions and hidden definitions in the
* above sections.
****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable* createHashTable(HashFunction hashFunction, unsigned int numBuckets) {
  // The hash table has to contain at least one bucket. Exit gracefully if
  // this condition is not met.
  if (numBuckets==0) {
    printf("Hash table has to contain at least 1 bucket...\n");
    exit(1);
  }

  // Allocate memory for the new HashTable struct on heap.
  HashTable* newTable = (HashTable*)malloc(sizeof(HashTable));

  // Initialize the components of the new HashTable struct.
  newTable->hash = hashFunction;
  newTable->num_buckets = numBuckets;
  newTable->buckets = (HashTableEntry**)malloc(numBuckets*sizeof(HashTableEntry*));

  // As the new buckets contain indeterminant values, init each bucket as NULL.
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {
    newTable->buckets[i] = NULL;
  }

  // Return the new HashTable struct.
  return newTable;
}

void destroyHashTable(HashTable* hashTable) {
    int i;  //for loop counter

    for (i = 0; i < hashTable->num_buckets; i++)        //important to note that i is a counter that iterates through the ROWS/BUCKETS, not the LinkedList
    {
        if (hashTable->buckets[i])                      //once in current bucket, if there is an entry at the beginning of the LinkedList
        {
          HashTableEntry* currentNode = hashTable->buckets[i];  //create pointer for this list that points to the current entry
          HashTableEntry* nextNode = currentNode->next;         //create a pointer to the next entry after the current entry that currentNode points to

            while(nextNode)     //while nextNode has an entry
            {
                free(currentNode->value);   //destroy currentNode's data
                free(currentNode);          //destroy currentNode pointer
                currentNode = nextNode;     //currentNode points to next entry
                nextNode = nextNode->next;  //nextNode points to the one beyond that
            }
            free(currentNode);              //this line is not necessary unless the while loop is not entered, in which case, currentNode IS the last node
        }
    }
    free(hashTable);    //free the buckets that make up the hashTable from the ground up
}

void* insertItem(HashTable* hashTable, unsigned int key, void* value) {

    int index = hashTable->hash(key);   //index found for which bucket the item to be inserted will be

    HashTableEntry* node = findItem(hashTable, key);    //this variable is used to check if an entry with the same key already exists

    if(node)        //if entry with key already exists, all that needs to be done is to overwrite the data at that entry
    {
        void* prevData = node->value;
        node->value = value;
        return prevData;        //return data if needed by user (The one overwritten)
    }

    HashTableEntry* currentNode = createHashTableEntry(key,value);  //if there is no entry with the same key as the one to be inserted, create a new entry

    if(!currentNode)        //if this pointer points at null due to edge case, return null
    {
        return NULL;
    }

    currentNode->next = hashTable->buckets[index];  //for inserting at the head, new head becomes this entry that was just created
    hashTable->buckets[index] = currentNode;    //set new head pointer
    return NULL;
  
}

void* getItem(HashTable* hashTable, unsigned int key) {

    HashTableEntry* node = findItem(hashTable, key);    //create pointer at an entry that finds the entry with arguments passed into getItem function

    if(node)    //if the node exists, return the data of that entry
    {
        return node->value;
    }

    return NULL;    //return NULL otherwise
}

void* removeItem(HashTable* hashTable, unsigned int key) {

    int index = hashTable->hash(key);       //index of bucket at which this item will be found

    HashTableEntry* currentNode = hashTable->buckets[index];    //pointer initialized at first item of list at found bucket



    //edge case for destroying first entry
    if(currentNode && currentNode->key == key)      //if currentNode points at an existing entry AND its key is the same as the one specified in the function call
    {                                               //then it is proven that the node to be removed is the first entry
        void* removedValue = currentNode->value;    //must be a void pointer because don't know datatype
        hashTable->buckets[index] = currentNode->next;  //head pointer now points to whatever currentNode's next pointer was pointing to
        free(currentNode);      //destroy currentNode
        return removedValue;    //give user the data that was part of the destroyed entry
    }

    //case for anything aside from first entry removal
    while(currentNode && currentNode->next)     //while anything in the bucket exists, and pointer to next entry exists (there is a current entry and a next entry)
    {
        if(currentNode->next->key == key)           //Checking ahead. The next node's key must be the one checked for conditional since we cannot traverse this list backwards
        {
            HashTableEntry* removedEntry = currentNode->next;   //pointer to entry to be removed is the currentNode's next entry
            void* removedValue = removedEntry->value;           //value that will be removed is next entry's value
            currentNode->next = currentNode->next->next;        //currentNode's next pointer now must be updated to point to the next, next entry, since the next is removed 
            free(removedEntry);                                 //destroy that entry
            return removedValue;                                //return its data in case needed
        }
        currentNode = currentNode->next;                        //While the currentNode's next entry's key is not the one to be removed, just iterate through list
    }   
    return NULL;                                                //end function
}

void deleteItem(HashTable* hashTable, unsigned int key) {
    
    int index = hashTable->hash(key);           //find index of bucket that the item to be deleted is in

    HashTableEntry* currentNode = hashTable->buckets[index];    //like remove item, initialize pointer for current node to head pointer

    

    if(findItem(hashTable,key) == NULL)     //if found item does not exist, end function; nothing to delete
    {
        return;
    }

    if (currentNode && currentNode->key == key)     //if current entry and it's key is the one to be deleted
    {                                               //proven that head pointer points to entry to be deleted. (Edge case)
        hashTable->buckets[index] = currentNode->next;      //set head pointer to the second item in the list

        free(currentNode->value);       //destroy data

        free(currentNode);              //destroy pointer to entry
        return;                         //end function
    }

    while(currentNode && currentNode->next)     //while current entry exists as does next entry
    {
        if(currentNode->next->key == key)           //check ahead. find if next entry is the one to be deleted
        {
            HashTableEntry* deletedEntry = currentNode->next;   //if so, create pointer to the entry to be deleted
            currentNode->next = currentNode->next->next;        //next pointer for current entry needs to be updated to point at next, next entry, since next will be deleted

            free(deletedEntry->value);      //destroy entry to be deleted's data
            free(deletedEntry);             //destroy entry to be deleted
            return;                         //end function
        }
        currentNode = currentNode->next;    //if not, (while pointer to the next entry's key is not the one to be deleted) just iterate through linkedlist until found
    }
}
