#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dram.h"

#define ACT_LATENCY 45
#define CAS_LATENCY 45
#define PRE_LATENCY 45
#define BUS_LATENCY 10

#define RB_SIZE 1024
#define DRAM_BANKS 16

extern MODE SIM_MODE;
extern uns64 CACHE_LINESIZE;
extern Flag DRAM_PAGE_POLICY;

DRAM* dram_new() {

    return ((DRAM*)calloc(1, sizeof(DRAM)));
   
}

uns64 dram_access(DRAM *dram, Addr lineaddr, Flag is_dram_write) {

    uns64 x = 100;

    if (SIM_MODE == SIM_MODE_B || SIM_MODE == SIM_MODE_A) {

      x = 100;

    } else {

      x = dram_access_mode_CDE(dram, lineaddr, is_dram_write);

    }


    if (is_dram_write) {

      dram -> stat_write_access++;
      dram -> stat_write_delay += x;

    } else {

      dram -> stat_read_access++;
      dram -> stat_read_delay += x;

    }

    return x;

}

uns64 dram_access_mode_CDE(DRAM *dram,Addr lineaddr, Flag is_dram_write) {

    uns64 bInd = (lineaddr / (RB_SIZE/CACHE_LINESIZE)) % DRAM_BANKS;
    uns64 row = (lineaddr / (RB_SIZE/CACHE_LINESIZE)) / DRAM_BANKS;

    uns64 delay = 0;

    if (DRAM_PAGE_POLICY == 1) {

      delay = ACT_LATENCY + CAS_LATENCY + BUS_LATENCY;

    } else {

      if (dram -> rbArray[bInd].valid && dram -> rbArray[bInd].rowID == row) {
        delay = CAS_LATENCY + BUS_LATENCY;
      } else if (dram -> rbArray[bInd].valid) {
        delay = PRE_LATENCY + ACT_LATENCY + CAS_LATENCY + BUS_LATENCY;
      } else {
        delay = ACT_LATENCY + CAS_LATENCY + BUS_LATENCY;
      }

    }
    dram -> rbArray[bInd].rowID = row;
    dram -> rbArray[bInd].valid = 1;
    
    return delay;

}
////////////////////////////////////////////////////////////////////
// ------------- DO NOT MODIFY THE PRINT STATS FUNCTION -------
////////////////////////////////////////////////////////////////////

void    dram_print_stats(DRAM *dram){
  double rddelay_avg=0;
  double wrdelay_avg=0;
  char header[256];
  sprintf(header, "DRAM");

  if(dram->stat_read_access){
    rddelay_avg=(double)(dram->stat_read_delay)/(double)(dram->stat_read_access);
  }

  if(dram->stat_write_access){
    wrdelay_avg=(double)(dram->stat_write_delay)/(double)(dram->stat_write_access);
  }

  printf("\n%s_READ_ACCESS\t\t : %10llu", header, dram->stat_read_access);
  printf("\n%s_WRITE_ACCESS\t\t : %10llu", header, dram->stat_write_access);
  printf("\n%s_READ_DELAY_AVG\t\t : %10.3f", header, rddelay_avg);
  printf("\n%s_WRITE_DELAY_AVG\t\t : %10.3f", header, wrdelay_avg);


}
