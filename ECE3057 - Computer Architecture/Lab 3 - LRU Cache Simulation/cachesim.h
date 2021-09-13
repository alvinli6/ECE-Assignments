/**
 * @author ECE 3057 TAs
 */
/**
 * Name: Alvin Li
 * GTID: 903476153
 */
#ifndef __CACHESIM_H
#define __CACHESIM_H

// Use these in code to make mem read/mem write/inst read related code more readable
#define MEMREAD 0
#define MEMWRITE 1
#define IFETCH 2

#include "lrustack.h"

// Please DO NOT CHANGE the following two typedefs
typedef unsigned long long addr_t;		// Data type to hold addresses
typedef unsigned long long counter_t;	// Data type to hold cache statistic variables

/**
 * Struct for a cache block. Feel free to change any of this if you want. 
 */
typedef struct cache_block_t {
	int tag;
	int valid;
	int dirty;
} cache_block_t;

/**
 * Struct for a cache set. Feel free to change any of this if you want. 
 */
typedef struct cache_set_t {
	int size;				// Number of blocks in this cache set
	lru_stack_t* stack;		// LRU Stack 
	cache_block_t* blocks;	// Array of cache block structs. You will need to
							// 	dynamically allocate based on number of blocks
							//	per set. 
} cache_set_t;

void cachesim_init(int block_size, int cache_size, int ways);
void cachesim_access(addr_t physical_add, int access_type);
void cachesim_cleanup(void);
void cachesim_print_stats(void);

#endif
