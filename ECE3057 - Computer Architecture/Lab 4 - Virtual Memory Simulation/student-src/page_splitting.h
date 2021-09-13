#pragma once
#include <math.h>
#include "paging.h"

/*  --------------------------------- PROBLEM 1 --------------------------------------
    Split the virtual address into VPN and offset.
    These macros will be used whenever you get a memory address from the CPU and need
    to translate it.

    HINT: Examine the global defines in pagesim.h, which will be
    necessary in implementing these functions.

    HINT: While these values will be powers of two, allowing you to use bit-wise
    arithmetic, consider using modulus division and integer division for
    an elegant solution.
    -----------------------------------------------------------------------------------
 */

/* Get the virtual page number from a virtual address. */
static inline vpn_t vaddr_vpn(vaddr_t addr) {
    //shift offsit bits out to get virtual page number bits
    return (addr >> OFFSET_LEN);                   /* FIXME */

}

/* Get the offset into the page from a virtual address. */
static inline uint16_t vaddr_offset(vaddr_t addr) {
    //the term addr is bitwise anded with is a mask by doing 2 to the power of the offset bit length - 1, there is a mask of 1's as long as the offset bit length
    return (addr & (((unsigned int)pow(2, OFFSET_LEN)) - 1));                   /* FIXME */

}
