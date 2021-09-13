/**
 * @author ECE 3057 TAs
 */

/**
 * Name: Alvin Li
 * GTID: 903476153
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachesim.h"
#include "lrustack.h" // include other header file

// Statistics you will need to keep track. DO NOT CHANGE THESE.
counter_t accesses = 0;     // Total number of cache accesses
counter_t hits = 0;         // Total number of cache hits
counter_t misses = 0;       // Total number of cache misses
counter_t writebacks = 0;   // Total number of writebacks

/**
 * Function to perform a very basic log2. It is not a full log function, 
 * but it is all that is needed for this assignment. The <math.h> log
 * function causes issues for some people, so we are providing this. 
 * 
 * @param x is the number you want the log of.
 * @returns Techinically, floor(log_2(x)). But for this lab, x should always be a power of 2.
 */
int simple_log_2(int x) {
    int val = 0;
    while (x > 1) {
        x /= 2;
        val++;
    }
    return val; 
}

//  Here are some global variables you may find useful to get you started.
//      Feel free to add/change anyting here.
cache_set_t* cache;     // Data structure for the cache
int block_size;         // Block size
int cache_size;         // Cache size
int ways;               // Ways
int num_sets;           // Number of sets
int num_offset_bits;    // Number of offset bits
int num_index_bits;     // Number of index bits. 

/**
 * Function to intialize your cache simulator with the given cache parameters. 
 * Note that we will only input valid parameters and all the inputs will always 
 * be a power of 2.
 * 
 * @param _block_size is the block size in bytes
 * @param _cache_size is the cache size in bytes
 * @param _ways is the associativity
 */
void cachesim_init(int _block_size, int _cache_size, int _ways) {
    // Set cache parameters to global variables
    block_size = _block_size;
    cache_size = _cache_size;
    ways = _ways;

    ////////////////////////////////////////////////////////////////////
    //  TODO: Write the rest of the code needed to initialize your cache
    //  simulator. Some of the things you may want to do are:
    //      - Calculate any values you need such as number of index bits.
    //      - Allocate any data structures you need.   
    ////////////////////////////////////////////////////////////////////

    int numBlocks = 1;
    numBlocks = cache_size / block_size; //use formula given to calculate # of blocks
    
    num_sets = numBlocks / ways; //continue with formula to find # of indices in cache array
    //this formula divides the amount of blocks by the "hashing" done in each block to create a set

    //allocate memory for a pointer to the cache
    cache = (cache_set_t*)malloc(num_sets * sizeof(cache_set_t));

    //run for loop through each set
    for (int i = 0; i < num_sets; i++) {

    	cache[i].size = ways;
    	cache[i].stack = init_lru_stack(ways);
    	cache[i].blocks = (cache_block_t*)malloc(ways * sizeof(cache_block_t)); //allocate memory equal to amount needed by a set (i.e. ways * block size) 

    	//run nested for loop through the ways of a set
    	for (int j = 0; j < ways; j++) {

    		cache[i].blocks[j].valid = 0;		//initialize all to 0
    		cache[i].blocks[j].tag = 0;
    		cache[i].blocks[j].dirty = 0;

    	}
    }
    //end function
}

/**
 * Function to perform a SINGLE memory access to your cache. In this function, 
 * you will need to update the required statistics (accesses, hits, misses, writebacks)
 * and update your cache data structure with any changes necessary.
 * 
 * @param physical_addr is the address to use for the memory access. 
 * @param access_type is the type of access - 0 (data read), 1 (data write) or 
 *      2 (instruction read). We have provided macros (MEMREAD, MEMWRITE, IFETCH)
 *      to reflect these values in cachesim.h so you can make your code more readable.
 */
void cachesim_access(addr_t physical_addr, int access_type) {
    ////////////////////////////////////////////////////////////////////
    //  TODO: Write the code needed to perform a cache access on your
    //  cache simulator. Some things to remember:
    //      - When it is a cache hit, you SHOULD NOT bring another cache 
    //        block in.
    //      - When it is a cache miss, you should bring a new cache block
    //        in. If the set is full, evict the LRU block.
    //      - Remember to update all the necessary statistics as necessary
    //      - Remember to correctly update your valid and dirty bits.  
    ////////////////////////////////////////////////////////////////////

    ++accesses;

    int tagIndex = physical_addr / block_size;
    int index = tagIndex % num_sets;
    int tag = tagIndex / num_sets;

    cache_block_t* setIndex = cache[index].blocks;

    for (int i = 0; i < ways; i++) {
    	//within a block, iterate until tag is found
    	if ((setIndex[i].tag == tag) && (setIndex[i].valid == 1)) {
    		//only hit if the tag matches and the block has readable data
    		++hits;

    		if(access_type == MEMWRITE) {

    			setIndex[i].dirty = 1; //assign dirty to this block if the access is a memwrite. Data integrity monitor

    		}

    		lru_stack_set_mru(cache[index].stack, i); //call function from lru that makes this the most recently used out of the set

    		return; //end function
    	}
    }

    ++misses; //in any other case, a miss has occurred.

    for (int i = 0; i < ways; i++) {
    	//first case of miss is if there is no data i.e. not a valid block
    	
    	if (setIndex[i].valid == 0) {
    		setIndex[i].valid = 1;		//there is now data after the miss
    		setIndex[i].tag = tag;		//the valid tag is now set in
    		
    		if (access_type == MEMWRITE) {
    			setIndex[i].dirty = 1;	//as above, if being written to, block becomes dirty data
    		}

    		lru_stack_set_mru(cache[index].stack, i); 	//set MRU

    		return;
    	}
    }

    //if this point is reached, cache miss, but all valid blocks
    //replace oldest "probably not necessary" data LRU block

    int lruIndex = lru_stack_get_lru(cache[index].stack);

    setIndex[lruIndex].tag = tag;
    if ((setIndex[lruIndex].valid == 1) && (setIndex[lruIndex].dirty == 1)) {

    	++writebacks;		//if the least recently accessed block has data and is dirty, then memory writebacks occurred

    }

    setIndex[lruIndex].valid = 1; //assign 1 regardless because the block now has data

    if (access_type == MEMWRITE) {
    	setIndex[lruIndex].dirty = 1; //index is dirty if memory write
    } else {
    	setIndex[lruIndex].dirty = 0; //for any other stage
    }

    lru_stack_set_mru(cache[index].stack, lruIndex); 	//call function to updata most recent used

    return;
}


/**
 * Function to free up any dynamically allocated memory you allocated
 */
void cachesim_cleanup() {
    ////////////////////////////////////////////////////////////////////
    //  TODO: Write the code to do any heap allocation cleanup
    ////////////////////////////////////////////////////////////////////

    for (int i = 0; i < num_sets; i++) {

    	free(cache[i].blocks);  //go through each set and free up memory that the blocks took
    	lru_stack_cleanup(cache[i].stack);	//free malloc'd stuff from init_lru
    }

    free(cache); //after data in all blocks/sets have been freed, free entire cache entity
}

/**
 * Function to print cache statistics
 * DO NOT update what this prints.
 */
void cachesim_print_stats() {
    printf("%llu, %llu, %llu, %llu\n", accesses, hits, misses, writebacks);  
}

/**
 * Function to open the trace file
 * You do not need to update this function. 
 */
FILE *open_trace(const char *filename) {
    return fopen(filename, "r");
}

/**
 * Read in next line of the trace
 * 
 * @param trace is the file handler for the trace
 * @return 0 when error or EOF and 1 otherwise. 
 */
int next_line(FILE* trace) {
    if (feof(trace) || ferror(trace)) return 0;
    else {
        int t;
        unsigned long long address, instr;
        fscanf(trace, "%d %llx %llx\n", &t, &address, &instr);
        cachesim_access(address, t);
    }
    return 1;
}

/**
 * Main function. See error message for usage. 
 * 
 * @param argc number of arguments
 * @param argv Argument values
 * @returns 0 on success. 
 */
int main(int argc, char **argv) {
    FILE *input;

    if (argc != 5) {
        fprintf(stderr, "Usage:\n  %s <trace> <block size(bytes)>"
                        " <cache size(bytes)> <ways>\n", argv[0]);
        return 1;
    }
    
    input = open_trace(argv[1]);
    cachesim_init(atol(argv[2]), atol(argv[3]), atol(argv[4]));
    while (next_line(input));
    cachesim_print_stats();
    cachesim_cleanup();
    fclose(input);
    return 0;
}

