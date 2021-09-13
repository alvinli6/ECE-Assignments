#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "cache.h"

extern uns64 cycle;
extern uns64 SWP_CORE0_WAYS;

Cache* cache_new(uns64 size, uns64 assocs, uns64 linesize, uns64 repl_policy) {

	  uns64 numSets = size / (assocs * linesize);		//cachesize / (ways * block_size) = # sets
	  Cache* newCache = (Cache*) calloc(1, sizeof(Cache));		// allocate pointer to 1 chunk from heap for the size of a defined Cache struct
	  newCache -> cache = (Cache_Set*) calloc(numSets, sizeof(Cache_Set));	//allocate space for an ARRAY of sets. Cache points to set[0]
	  newCache -> ways = assocs;	//initialize these 3
	  newCache -> numSets = numSets;
	  newCache -> repl_policy = repl_policy;
	  
	  return newCache;	//output constructed cache pointer

}

Flag cache_access(Cache *c, Addr lineaddr, uns is_write, uns core_id) {
  
	  uns64 tag = lineaddr /  c -> numSets;		//access address / numSets = tag value
	  uns64 setIndex = lineaddr % c -> numSets;	//access address % numSets = the set we go in to look for block

	  if (is_write) {

	    c -> stat_write_access++;

	  } else {

	    c->stat_read_access++;

	  }
	  
	  for (uns i = 0; i < c -> ways; i++) {

		    Cache_Line tmp = c -> cache[setIndex].blocks[i];
		    
		    if (tmp.valid) {
		      
			      if (tmp.tag == tag && tmp.core_id == core_id) {
			       
			        c -> cache[setIndex].blocks[i].last_access_time = cycle;
			       
			        if (is_write) {

			          c -> cache[setIndex].blocks[i].dirty = 1;

			        }

			        return HIT;

			      }

		    }

	  }

	  if (is_write) {

	    c -> stat_write_miss++;

	  } else {

	    c -> stat_read_miss++;

	  }

	  return MISS;

}

void cache_install(Cache *c, Addr lineaddr, uns is_write, uns core_id) {

	  uns64 tag = lineaddr / c -> numSets;
	  uns64 setIndex = lineaddr % c -> numSets;
	  
	  uns vicIndex = cache_find_victim(c, setIndex, core_id);
	  c -> lastEvicted = c -> cache[setIndex].blocks[vicIndex];

	  if (c -> lastEvicted.valid && c -> lastEvicted.dirty) {

	    c -> stat_dirty_evicts++; //writebacks++

	  }
	  
	  
	  c -> cache[setIndex].blocks[vicIndex].valid = 1;
	  c -> cache[setIndex].blocks[vicIndex].dirty = is_write;
	  c -> cache[setIndex].blocks[vicIndex].tag = tag;
	  c -> cache[setIndex].blocks[vicIndex].core_id = core_id;
	  c -> cache[setIndex].blocks[vicIndex].last_access_time = cycle;
	  
}

uns cache_find_victim(Cache *c, uns set_index, uns core_id) {
  
  Cache_Set setIndex = c->cache[set_index];

  for (uns i = 0; i < c->ways; i++) {

    if (!setIndex.blocks[i].valid) {

      return i;

    }

  }
  
  if (c -> repl_policy == 1) {

    uns randIndex = (uns)(rand() % c -> ways);	//random num to find which to evict
    return randIndex;

  } else if (c -> repl_policy == 0) {

	    uns lruIndex = 0;
	    uns64 lruTime = setIndex.blocks[0].last_access_time;
	    for (uns i = 1; i < c->ways; i++) {		//iterate until find real oldest block/time

	      uns64 tmp = setIndex.blocks[i].last_access_time;

		      if (tmp < lruTime) {

		        lruTime = tmp;
		        lruIndex = i;

		      }

	    }

	    return lruIndex;

  } else {      

	    uns vicIndex = 0;
	    
	    if (check_swap(c, set_index, core_id)) { 
	      
	      uns64 vicTime = cycle;

	      for (uns i = 0; i < c -> ways; i++) {			// same logic as lru

	        uns64 tmp = setIndex.blocks[i].last_access_time;
	        uns tmp_core_id = setIndex.blocks[i].core_id;
	        if (tmp_core_id != core_id && tmp < vicTime) {

	          vicTime = tmp;
	          vicIndex = i;

	        }

	      }

	    } else {                                     
	     
	      uns64 vicTime = cycle;

	      for (uns i = 0; i < c -> ways; i++) {

	        	uns64 tmp = setIndex.blocks[i].last_access_time;
	        	uns tmp_core_id = setIndex.blocks[i].core_id;
	        		if (tmp_core_id == core_id && tmp < vicTime) {

	          			vicTime = tmp;
	          			vicIndex = i;

	        		}

	      }
	    }

    	return vicIndex;

  }

}


bool check_swap(Cache * c, uns set_index, uns core_id) {

	  Cache_Set setIndex = c -> cache[set_index];
	  uns filled = 0;
	  uns64 swaps = SWP_CORE0_WAYS;

	  if (core_id) {
	    swaps = c -> ways - SWP_CORE0_WAYS;
	  }

	  for (uns i = 0; i < c -> ways; i++) {
	   
	    if (setIndex.blocks[i].valid && setIndex.blocks[i].core_id == core_id) {

	      filled++;

	    }

	    if (filled >= swaps) {

	      return false;

	    }

	  }
	  
	  return true;

}

////////////////////////////////////////////////////////////////////
// ------------- DO NOT MODIFY THE PRINT STATS FUNCTION -----------
////////////////////////////////////////////////////////////////////

void    cache_print_stats    (Cache *c, char *header){
  double read_mr =0;
  double write_mr =0;

  if(c->stat_read_access){
    read_mr=(double)(c->stat_read_miss)/(double)(c->stat_read_access);
  }

  if(c->stat_write_access){
    write_mr=(double)(c->stat_write_miss)/(double)(c->stat_write_access);
  }

  printf("\n%s_READ_ACCESS    \t\t : %10llu", header, c->stat_read_access);
  printf("\n%s_WRITE_ACCESS   \t\t : %10llu", header, c->stat_write_access);
  printf("\n%s_READ_MISS      \t\t : %10llu", header, c->stat_read_miss);
  printf("\n%s_WRITE_MISS     \t\t : %10llu", header, c->stat_write_miss);
  printf("\n%s_READ_MISS_PERC  \t\t : %10.3f", header, 100*read_mr);
  printf("\n%s_WRITE_MISS_PERC \t\t : %10.3f", header, 100*write_mr);
  printf("\n%s_DIRTY_EVICTS   \t\t : %10llu", header, c->stat_dirty_evicts);

  printf("\n");
}




