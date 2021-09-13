#ifndef DRAM_H
#define DRAM_H
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

typedef struct RowBuf_Entry {

    int valid;
    uns64 rowID;

} RowBuf_Entry;

typedef struct DRAM {

    RowBuf_Entry rbArray[256];

    //stats
    uns64 stat_read_access;
    uns64 stat_write_access;
    uns64 stat_read_delay;
    uns64 stat_write_delay;

} DRAM;

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

DRAM 		*dram_new();
void		 dram_print_stats(DRAM *dram);
uns64 		 dram_access(DRAM *dram, Addr lineaddr, Flag is_dram_write);
uns64 		 dram_access_mode_CDE(DRAM *dram, Addr lineaddr, Flag is_dram_write);
#endif // DRAM_H