#ifndef MEMSYS_H
#define MEMSYS_H

#include "types.h"
#include "cache.h"
#include "dram.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef struct Memsys   Memsys;

struct Memsys {
  Cache *dcache;  // For Part A
  Cache *icache;  // For Part A,B,C

  Cache *dcache_coreid[MAX_CORES];  // For Part D,E
  Cache *icache_coreid[MAX_CORES];  // For Part D,E
  
  Cache *l2cache; // For Part A,B,C,D,E
  DRAM  *dram;    // For Part C,D,E

   // stats 
  uns64 stat_ifetch_access;
  uns64 stat_load_access;
  uns64 stat_store_access;
  uns64 stat_ifetch_delay;
  uns64 stat_load_delay;
  uns64 stat_store_delay;
};



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

Memsys *memsys_new();
void    memsys_print_stats(Memsys *sys);

uns64   memsys_access(Memsys *sys, Addr addr, Access_Type type, uns core_id);
uns64   memsys_access_modeA(Memsys *sys, Addr lineaddr, Access_Type type, uns core_id);
uns64   memsys_access_modeBC(Memsys *sys, Addr lineaddr, Access_Type type, uns core_id);
uns64   memsys_access_modeDE(Memsys *sys, Addr lineaddr, Access_Type type, uns core_id);


// For mode B/C/D/E you must use this function to access L2 
uns64   memsys_L2_access(Memsys *sys, Addr lineaddr, Flag is_writeback, uns core_id);
uns64   memsys_L2_access_multicore(Memsys *sys, Addr lineaddr, Flag is_writeback, uns core_id);

// This function can convert VPN to PFN
uns64 memsys_convert_vpn_to_pfn(Memsys *sys, uns64 vpn, uns core_id);

///////////////////////////////////////////////////////////////////

#endif // MEMSYS_H
