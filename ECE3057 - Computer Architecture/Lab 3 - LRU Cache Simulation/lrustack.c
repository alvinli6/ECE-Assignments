/**
 * @author ECE 3057 TAs
 */
/**
 * Name: Alvin Li
 * GTID: 903476153
 */
#include <stdlib.h>
#include "lrustack.h"

/**
 * This file contains some starter code to get you started on your LRU implementation. 
 * You are free to implement it however you see fit. You can design it to emulate how this
 * would be implemented in hardware, or design a purely software stack. 
 * 
 * We have broken down the LRU stack's
 * job/interface into two parts:
 *  - get LRU: gets the current index of the LRU block
 *  - set MRU: sets a certain block's index as the MRU. 
 * If you implement it using these suggestions, you will be able to test your LRU implementation
 * using lrustacktest.c
 * 
 * NOTES: 
 *      - You are not required to use this LRU interface. Feel free to design it from scratch if 
 *      that is better for you.
 *      - This will not behave like your traditional LIFO stack  
 */

/**
 * Function to initialize an LRU stack for a cache set with a given <size>. This function
 * creates the LRU stack. 
 * 
 * @param size is the size of the LRU stack to initialize. 
 * @return the dynamically allocated stack. 
 */
lru_stack_t* init_lru_stack(int size) {
    //  Use malloc to dynamically allocate a lru_stack_t
	lru_stack_t* stack = (lru_stack_t*) malloc(sizeof(lru_stack_t));
    //  Set the stack size the caller passed in
	stack->size = size;
    
    ////////////////////////////////////////////////////////////////////
    //  TODO: Write any other code needed to initialize your LRU Stack. 
    //  Essentially write any initializations needed for anything you
    //  added to lru_stack_t.
    ////////////////////////////////////////////////////////////////////

    stack->priorityArray = (int*)malloc(size * sizeof(int)); //create pointer to block array

    for (int i = 0; i < stack->size; i++) {
        stack->priorityArray[i] = i; //iterate through stack and initialize values
    }

	return stack;
}

/**
 * Function to get the index of the least recently used cache block, as indicated by <stack>.
 * This operation should not change/mutate your LRU stack. 
 * 
 * @param stack is the stack to run the operation on.
 * @return the index of the LRU cache block.
 */
int lru_stack_get_lru(lru_stack_t* stack) {
    ////////////////////////////////////////////////////////////////////
    //  TODO: Write code to get the index of the LRU block from the LRU 
    //  Stack. 
    ////////////////////////////////////////////////////////////////////
    
    int lru_index = stack->size - 1; //maximum possible value in stack

    //in my implmentation, most recent is 0, least is max
    for (int i = 0; i < stack->size; i++) {
        if (stack->priorityArray[i] == lru_index) {
            return i;       //iterate through until lru_index is found, return that index
        } 
    }

}

/**
 * Function to mark the cache block with index <n> as MRU in <stack>. This operation should
 * change/mutate the LRU stack.
 * 
 * @param stack is the stack to run the operation on.
 * @param n the index to promote to MRU.  
 */
void lru_stack_set_mru(lru_stack_t* stack, int n) {
	////////////////////////////////////////////////////////////////////
    //  TODO: Write code to set the passed in block index  as the MRU 
    //  element in the LRU Stack. 
    ////////////////////////////////////////////////////////////////////

    for (int i = 0; i < stack->size; i++) { //iterate through all of priority array
        if ((stack->priorityArray[i] < stack->priorityArray[n]) && (i != n)) {  //only increment priority bit of
            stack->priorityArray[i] = stack->priorityArray[i] + 1;              //the index that contain bits less than the one being set to MRU
        }   //i.e. if 2 is changed to 0, only original 1 and 0 become older, as 3 is still LRU
    }

    stack->priorityArray[n] = 0; //set MRU
}

/**
 * Function to free up any memory you dynamically allocated for <stack>
 * 
 * @param stack the stack to free
 */
void lru_stack_cleanup(lru_stack_t* stack) {
    ////////////////////////////////////////////////////////////////////
    //  TODO: Write any code if you need to do additional heap allocation
    //  cleanup
    ////////////////////////////////////////////////////////////////////

    free(stack->priorityArray); //always free parts before the whole

    free(stack);        // Free the stack struct we malloc'd
}