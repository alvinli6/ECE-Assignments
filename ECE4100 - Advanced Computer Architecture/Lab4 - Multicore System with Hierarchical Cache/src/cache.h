#ifndef CACHE_H
#define CACHE_H

#define MAX_WAYS 16

#include "types.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// Define the Data Structures here with correct field (Look at Appendix A for more details)
//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Cache_Line {

	int valid;		// valid bit
	int dirty;		// dirty bit
	uns64 tag;		// tag for this block 
	uns core_id;	// which core?
	uns64 last_access_time;		// for eviction policy such as LRU, kick out oldest

} Cache_Line;

typedef struct Cache_Set {

	Cache_Line blocks[MAX_WAYS];		//cache sets are array of blocks, like how cache is array of sets

} Cache_Set;

typedef struct Cache {

	Cache_Set* cache;		//pointer to cache struct. The first set in the cache

	uns64 ways;				//associativity
	uns64 numSets;			//sets
	uns64 repl_policy;		//eviction policy
	Cache_Line lastEvicted;	//Keep track of last block kicked out
	//stats
	uns64 stat_read_access;
	uns64 stat_write_access;
	uns64 stat_read_miss;
	uns64 stat_write_miss;
	uns64 stat_dirty_evicts;	//writebacks

} Cache;


//////////////////////////////////////////////////////////////////////////////////////////////
// Following functions might be helpful(Look at Appendix A for more details)
//////////////////////////////////////////////////////////////////////////////////////////////


Cache  *cache_new			 (uns64 size, uns64 assocs, uns64 linesize, uns64 repl_policy);
Flag    cache_access         (Cache *c, Addr lineaddr, uns is_write, uns core_id);
void    cache_install        (Cache *c, Addr lineaddr, uns is_write, uns core_id);
void    cache_print_stats    (Cache *c, char *header);
uns     cache_find_victim    (Cache *c, uns set_index, uns core_id);
bool	check_swap			 (Cache *c, uns index, uns core_id);	//index is the set. This function checks for swap space

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#endif // CACHE_H
