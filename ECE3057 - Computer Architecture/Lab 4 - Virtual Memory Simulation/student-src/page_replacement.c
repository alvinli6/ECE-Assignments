#include "types.h"
#include "pagesim.h"
#include "paging.h"
#include "swapops.h"
#include "stats.h"
#include "util.h"

pfn_t select_victim_frame(void);


/*  --------------------------------- PROBLEM 7 --------------------------------------
    Make a free frame for the system to use.

    You will first call the page replacement algorithm to identify an
    "available" frame in the system.

    In some cases, the replacement algorithm will return a frame that
    is in use by another page mapping. In these cases, you must "evict"
    the frame by using the frame table to find the original mapping and
    setting it to invalid. If the frame is dirty, write its data to swap!
 * ----------------------------------------------------------------------------------
 */
pfn_t free_frame(void) {
    pfn_t victim_pfn;

    /* Call your function to find a frame to use, either one that is
       unused or has been selected as a "victim" to take from another
       mapping. */
    victim_pfn = select_victim_frame();

    /*
     * If victim frame is currently mapped:
     *
     * 1) Look up the corresponding page table entry
     * 2) If the entry is dirty, write it to disk with swap_write()
     * 3) Mark the original page table entry as invalid
     */

    /* If the victim is in use, we must evict it first */
    if (frame_table[victim_pfn].mapped == 1) {
        pfn_t curr = frame_table[victim_pfn].process->saved_ptbr;

        pte_t* pageTable = (pte_t*)(mem + (curr * PAGE_SIZE));
        pte_t* pageTableEntry = pageTable + frame_table[victim_pfn].vpn;

        if (pageTableEntry->dirty == 1) {

            void* change = (void*)(mem + victim_pfn * PAGE_SIZE);
            swap_write(pageTableEntry, change);
            stats.writebacks++;
            pageTableEntry->dirty = 0;
        }
    

    pageTableEntry->valid = 0;
    frame_table[victim_pfn].mapped = 0;
    }
    /* Return the pfn */
    return victim_pfn;
}



pfn_t select_victim_frame() {
    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++) {
        if (!frame_table[i].protected && !frame_table[i].mapped) {
            return i;
        }
    }

    if (replacement == RANDOM) {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t last_unprotected = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                last_unprotected = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (last_unprotected < NUM_FRAMES) {
            return last_unprotected;
        }
    } 

    /* If every frame is protected, give up. This should never happen
       on the traces we provide you. */
    panic("System ran out of memory\n");
    exit(1);
}
