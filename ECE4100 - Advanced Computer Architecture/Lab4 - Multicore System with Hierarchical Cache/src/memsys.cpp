#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "memsys.h"

#define PAGE_SIZE 4096

//---- Cache Latencies  ------

#define DCACHE_HIT_LATENCY   1
#define ICACHE_HIT_LATENCY   1
#define L2CACHE_HIT_LATENCY  10

extern MODE   SIM_MODE;
extern uns64  CACHE_LINESIZE;
extern uns64  REPL_POLICY;

extern uns64  DCACHE_SIZE; 
extern uns64  DCACHE_ASSOC; 
extern uns64  ICACHE_SIZE; 
extern uns64  ICACHE_ASSOC; 
extern uns64  L2CACHE_SIZE; 
extern uns64  L2CACHE_ASSOC;
extern uns64  L2CACHE_REPL;
extern uns64  NUM_CORES;
extern uns64 	cycle;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


Memsys *memsys_new(void) {

    Memsys* sys = (Memsys*)calloc (1, sizeof(Memsys));

    if(SIM_MODE==SIM_MODE_A) {

      sys -> dcache = cache_new(DCACHE_SIZE, DCACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);

    }

    if(SIM_MODE==SIM_MODE_B) {

      sys -> dcache = cache_new(DCACHE_SIZE, DCACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
      sys -> icache = cache_new(ICACHE_SIZE, ICACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
      sys -> l2cache = cache_new(L2CACHE_SIZE, L2CACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
      sys -> dram = dram_new();

    }

    if(SIM_MODE==SIM_MODE_C) {

      sys -> dcache = cache_new(DCACHE_SIZE, DCACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
      sys -> icache = cache_new(ICACHE_SIZE, ICACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
      sys -> l2cache = cache_new(L2CACHE_SIZE, L2CACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
      sys -> dram = dram_new();

    }

    if( (SIM_MODE==SIM_MODE_D) || (SIM_MODE==SIM_MODE_E)) {

      sys -> l2cache = cache_new(L2CACHE_SIZE, L2CACHE_ASSOC, CACHE_LINESIZE, L2CACHE_REPL);
      sys -> dram = dram_new();

      for(uns i = 0; i < NUM_CORES; i++) {
        sys -> dcache_coreid[i] = cache_new(DCACHE_SIZE, DCACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
        sys -> icache_coreid[i] = cache_new(ICACHE_SIZE, ICACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
      }
    }

  return sys;
}


////////////////////////////////////////////////////////////////////
// This function takes an ifetch/ldst access and returns the delay
////////////////////////////////////////////////////////////////////

uns64 memsys_access(Memsys *sys, Addr addr, Access_Type type, uns core_id)
{
  uns delay=0;


  // all cache transactions happen at line granularity, so get lineaddr
  Addr lineaddr=addr/CACHE_LINESIZE;

  if(SIM_MODE==SIM_MODE_A){
    delay = memsys_access_modeA(sys,lineaddr,type, core_id);
  }

  if((SIM_MODE==SIM_MODE_B)||(SIM_MODE==SIM_MODE_C)){
    delay = memsys_access_modeBC(sys,lineaddr,type, core_id);
  }

  if((SIM_MODE==SIM_MODE_D)||(SIM_MODE==SIM_MODE_E)){
    delay = memsys_access_modeDE(sys,lineaddr,type, core_id);
  }
  
  //update the stats
  if(type==ACCESS_TYPE_IFETCH){
    sys->stat_ifetch_access++;
    sys->stat_ifetch_delay+=delay;
  }

  if(type==ACCESS_TYPE_LOAD){
    sys->stat_load_access++;
    sys->stat_load_delay+=delay;
  }

  if(type==ACCESS_TYPE_STORE){
    sys->stat_store_access++;
    sys->stat_store_delay+=delay;
  }


  return delay;
}



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void memsys_print_stats(Memsys *sys)
{
  char header[256];
  sprintf(header, "MEMSYS");

  double ifetch_delay_avg=0;
  double load_delay_avg=0;
  double store_delay_avg=0;

  if(sys->stat_ifetch_access){
    ifetch_delay_avg = (double)(sys->stat_ifetch_delay)/(double)(sys->stat_ifetch_access);
  }

  if(sys->stat_load_access){
    load_delay_avg = (double)(sys->stat_load_delay)/(double)(sys->stat_load_access);
  }

  if(sys->stat_store_access){
    store_delay_avg = (double)(sys->stat_store_delay)/(double)(sys->stat_store_access);
  }


  printf("\n");
  printf("\n%s_IFETCH_ACCESS  \t\t : %10llu",  header, sys->stat_ifetch_access);
  printf("\n%s_LOAD_ACCESS    \t\t : %10llu",  header, sys->stat_load_access);
  printf("\n%s_STORE_ACCESS   \t\t : %10llu",  header, sys->stat_store_access);
  printf("\n%s_IFETCH_AVGDELAY\t\t : %10.3f",  header, ifetch_delay_avg);
  printf("\n%s_LOAD_AVGDELAY  \t\t : %10.3f",  header, load_delay_avg);
  printf("\n%s_STORE_AVGDELAY \t\t : %10.3f",  header, store_delay_avg);
  printf("\n");

   if(SIM_MODE==SIM_MODE_A){
    sprintf(header, "DCACHE");
    cache_print_stats(sys->dcache, header);
  }
  
  if((SIM_MODE==SIM_MODE_B)||(SIM_MODE==SIM_MODE_C)){
    sprintf(header, "ICACHE");
	  cache_print_stats(sys->icache, header);
	  sprintf(header, "DCACHE");
    cache_print_stats(sys->dcache, header);
	  sprintf(header, "L2CACHE");
    cache_print_stats(sys->l2cache, header);
    dram_print_stats(sys->dram);
  }

  if((SIM_MODE==SIM_MODE_D)||(SIM_MODE==SIM_MODE_E)){
    assert(NUM_CORES==2); //Hardcoded
	sprintf(header, "ICACHE_0");
    cache_print_stats(sys->icache_coreid[0], header);
	sprintf(header, "DCACHE_0");
    cache_print_stats(sys->dcache_coreid[0], header);
	sprintf(header, "ICACHE_1");
    cache_print_stats(sys->icache_coreid[1], header);
	sprintf(header, "DCACHE_1");
    cache_print_stats(sys->dcache_coreid[1], header);
	sprintf(header, "L2CACHE");
    cache_print_stats(sys->l2cache, header);
    dram_print_stats(sys->dram);
    
  }

}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

uns64 memsys_access_modeA(Memsys *sys, Addr lineaddr, Access_Type type, uns core_id){
 Flag dCacheNeeded=FALSE;
  Flag is_write=FALSE;
  
  if(type == ACCESS_TYPE_IFETCH){
    // no icache in this mode
  }
    
  if(type == ACCESS_TYPE_LOAD){
    dCacheNeeded=TRUE;
    is_write=FALSE;
  }
  
  if(type == ACCESS_TYPE_STORE){
    dCacheNeeded=TRUE;
    is_write=TRUE;
  }

  if(dCacheNeeded){
    Flag outcome=cache_access(sys->dcache, lineaddr, is_write, core_id);
    if(outcome==MISS){
      cache_install(sys->dcache, lineaddr, is_write, core_id);
    }
  }

  // timing is not simulated in Part A
  return 0;
}


uns64 memsys_access_modeBC(Memsys *sys, Addr lineaddr, Access_Type type,uns core_id){
  uns64 delay=0;   
  
  Flag is_write = FALSE;
  Flag dCacheNeeded = FALSE;

  if(type == ACCESS_TYPE_IFETCH) {

    delay = ICACHE_HIT_LATENCY;
    is_write = FALSE;

    Flag outcome = cache_access(sys -> icache, lineaddr, is_write, core_id);

    if(outcome==MISS) {
      uns64 l2Time = memsys_L2_access(sys, lineaddr, FALSE, core_id);
      delay += l2Time;
      cache_install(sys -> icache, lineaddr, is_write, core_id);

      if(sys -> icache -> lastEvicted.valid && sys -> icache -> lastEvicted.dirty) { //in the case of a writeback
        
        uns64 l2Address = ((sys -> icache -> lastEvicted.tag * sys -> icache -> numSets) | (lineaddr % sys -> icache -> numSets));
        memsys_L2_access(sys, l2Address, TRUE, sys -> icache -> lastEvicted.core_id);

      }
    }
    return delay;
  }


  //--------------------------//

  if(type == ACCESS_TYPE_LOAD){
 
    dCacheNeeded=TRUE;
    is_write=FALSE;

  }

  if(type == ACCESS_TYPE_STORE){

    dCacheNeeded=TRUE;
    is_write=TRUE;

  }
  if(dCacheNeeded){

    delay = DCACHE_HIT_LATENCY;
    Flag outcome = cache_access(sys -> dcache, lineaddr, is_write, core_id);

    if(outcome == MISS){
    
      uns64 l2Time = memsys_L2_access(sys, lineaddr, FALSE, core_id);
      delay += l2Time;

      cache_install(sys->dcache, lineaddr, is_write, core_id);

      if(sys -> dcache -> lastEvicted.valid && sys -> dcache -> lastEvicted.dirty) {
        
        uns64 l2Address = ((sys -> dcache -> lastEvicted.tag * sys -> dcache -> numSets) | (lineaddr % sys -> dcache -> numSets));
        memsys_L2_access(sys, l2Address, TRUE, sys -> dcache -> lastEvicted.core_id);

      }
    }
    return delay;
  }
  return delay;
}

uns64   memsys_L2_access(Memsys *sys, Addr lineaddr, Flag is_writeback, uns core_id){
  uns64 delay = L2CACHE_HIT_LATENCY;

  //To get the delay of L2 MISS, you must use the dram_access() function
  //To perform writebacks to memory, you must use the dram_access() function
  //This will help us track your memory reads and memory writes
  
  Flag outcome = cache_access(sys -> l2cache, lineaddr, is_writeback, core_id);

  if (outcome == MISS) {

    uns64 DRAMdelay = dram_access(sys -> dram, lineaddr, FALSE);
    delay += DRAMdelay;

    cache_install(sys -> l2cache, lineaddr, is_writeback, core_id);

    if (sys -> l2cache -> lastEvicted.valid && sys -> l2cache -> lastEvicted.dirty) {  //in the case of a writeback
      
      uns64 evictedAddress = ((sys -> l2cache -> lastEvicted.tag * sys -> l2cache -> numSets) | (lineaddr % sys -> l2cache -> numSets));
      dram_access(sys -> dram, evictedAddress, TRUE);

    }

  }
  return delay;
}

/////////////////////////////////////////////////////////////////////
// This function converts virtual page number (VPN) to physical frame
// number (PFN).  Note, you will need additional operations to obtain
// VPN from lineaddr and to get physical lineaddr using PFN. 
/////////////////////////////////////////////////////////////////////

uns64 memsys_convert_vpn_to_pfn(Memsys *sys, uns64 vpn, uns core_id){
  uns64 tail = vpn & 0x000fffff;
  uns64 head = vpn >> 20;
  uns64 pfn  = tail + (core_id << 21) + (head << 21);
  assert(NUM_CORES==2);
  return pfn;
}

////////////////////////////////////////////////////////////////////
// --------------- DO NOT CHANGE THE CODE ABOVE THIS LINE ----------
////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////
// For Mode D/E you will use per-core ICACHE and DCACHE
// ----- YOU NEED TO WRITE THIS FUNCTION AND UPDATE DELAY ----------
/////////////////////////////////////////////////////////////////////



uns64 memsys_access_modeDE(Memsys *sys, Addr v_lineaddr, Access_Type type,uns core_id){
  uns64 delay=0;
  Addr p_lineaddr=0;
	
  p_lineaddr = memsys_convert_vpn_to_pfn(sys, v_lineaddr, core_id);
  
  // TODO: First convert lineaddr from virtual (v) to physical (p) using the
  // function memsys_convert_vpn_to_pfn. Page size is defined to be 4KB.
  // NOTE: VPN_to_PFN operates at page granularity and returns page addr

  Flag isWrite = FALSE;
  Flag needsDcache = FALSE;
  if(type == ACCESS_TYPE_IFETCH) {

    delay = ICACHE_HIT_LATENCY;
    isWrite = FALSE;
    Flag result = cache_access(sys -> icache_coreid[core_id], p_lineaddr, isWrite, core_id);

    if(result == MISS) {
      
      uns64 l2latency = memsys_L2_access(sys, p_lineaddr, FALSE, core_id);
      delay += l2latency;
      cache_install(sys -> icache_coreid[core_id], p_lineaddr, isWrite, core_id);

      if(sys -> icache_coreid[core_id] -> lastEvicted.valid && sys -> icache_coreid[core_id] -> lastEvicted.dirty) {
       
        uns64 l2Addr = ((sys -> icache_coreid[core_id] -> lastEvicted.tag * sys -> icache_coreid[core_id] -> numSets) | (p_lineaddr % sys -> icache_coreid[core_id] -> numSets));
        memsys_L2_access(sys, l2Addr, TRUE, sys->icache_coreid[core_id] -> lastEvicted.core_id);

      }

    }

    return delay;
  }


  //-----------------------------------//
  if(type == ACCESS_TYPE_LOAD){

    needsDcache = TRUE;
    isWrite=FALSE;

  }
  if(type == ACCESS_TYPE_STORE){

    needsDcache = TRUE;
    isWrite=TRUE;

  }

  if(needsDcache) {

    delay = DCACHE_HIT_LATENCY;
    Flag result = cache_access(sys -> dcache_coreid[core_id], p_lineaddr, isWrite, core_id);

    if(result == MISS){
      
      uns64 l2latency = memsys_L2_access(sys, p_lineaddr, FALSE, core_id);
      delay += l2latency;
      cache_install(sys -> dcache_coreid[core_id], p_lineaddr, isWrite, core_id);

      if(sys -> dcache_coreid[core_id]-> lastEvicted.valid && sys -> dcache_coreid[core_id] -> lastEvicted.dirty) {
        
        uns64 l2Addr = ((sys -> dcache_coreid[core_id] -> lastEvicted.tag * sys -> dcache_coreid[core_id] -> numSets) | (p_lineaddr % sys -> dcache_coreid[core_id] -> numSets));
        memsys_L2_access(sys, l2Addr, TRUE, sys -> dcache_coreid[core_id] -> lastEvicted.core_id);

      }

    }

    return delay;

  }

  return delay;

}


/////////////////////////////////////////////////////////////////////
// This function is called on ICACHE miss, DCACHE miss, DCACHE writeback
// ----- YOU NEED TO WRITE THIS FUNCTION AND UPDATE DELAY ----------
/////////////////////////////////////////////////////////////////////

uns64   memsys_L2_access_multicore(Memsys *sys, Addr lineaddr, Flag is_writeback, uns core_id){
  uns64 delay = L2CACHE_HIT_LATENCY;
	
  //To get the delay of L2 MISS, you must use the dram_access() function
  //To perform writebacks to memory, you must use the dram_access() function
  //This will help us track your memory reads and memory writes
	
  Flag result = cache_access(sys -> l2cache, lineaddr, is_writeback, core_id);
  if (result == MISS) {

    uns64 dramLatency = dram_access(sys -> dram, lineaddr, FALSE);
    delay += dramLatency;
    cache_install(sys -> l2cache, lineaddr, is_writeback, core_id);

    if(sys -> l2cache -> lastEvicted.valid && sys -> l2cache -> lastEvicted.dirty) {
      
      uns64 evictedAddr = ((sys -> l2cache -> lastEvicted.tag * sys -> l2cache -> numSets) | (lineaddr % sys -> l2cache -> numSets));
      dram_access(sys -> dram, evictedAddr, TRUE);

    }

  }

  return delay;

}

