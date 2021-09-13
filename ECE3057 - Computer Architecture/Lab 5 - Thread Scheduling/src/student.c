/*
 * student.c
 * Multithreaded OS Simulation for CS 2200 and ECE 3057
 *
 * This file contains the CPU scheduler for the simulation.
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "os-sim.h"

/** Function prototypes **/
extern void idle(unsigned int cpu_id);
extern void preempt(unsigned int cpu_id);
extern void yield(unsigned int cpu_id);
extern void terminate(unsigned int cpu_id);
extern void wake_up(pcb_t *process);
static void push(pcb_t *pcb);
static pcb_t* pop();

/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t current_mutex;

static pcb_t *head;
static pthread_mutex_t readyMutex;
static pthread_cond_t notIdle;
static int timeSlice;
static int cpu_count;
static char algorithm;

/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running process indexed by the cpu id. 
 *	See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{
    pcb_t *pcb = pop();                 //pops off next pointer at a process
    if (pcb) {
        pcb->state = PROCESS_RUNNING;   //if process exists, set to running
    }
    pthread_mutex_lock(&current_mutex);     //lock current
    current[cpu_id] = pcb;                  //the cpu passed in as argument now gets this process
    pthread_mutex_unlock(&current_mutex);   //unlock after access done
    context_switch(cpu_id, pcb, timeSlice); //context switch called as instructed
}

static void push(pcb_t *pcb) {

    pthread_mutex_lock(&readyMutex);    //lock because adding something to queue
    pcb_t *curr = head;                 //current pointer to traverse queue/list
    if (!curr) {    //if null, nothing exists in queue, just push argument onto head of empty queue
        head = pcb;
    } else {    //otherwise

        while (curr->next) {    //traverse until next is null/at end of list
            curr = curr->next;
        }
        curr->next = pcb;       //add pcb once end is found

    }
    pcb->next = NULL;   //reassign pointer
    pthread_cond_broadcast(&notIdle);   
    pthread_mutex_unlock(&readyMutex);  //unlock when done with queue modification

}

static pcb_t* pop() {

    pcb_t *poppedPcb;
    pcb_t *curr;
    pcb_t *prevNode;

    unsigned int longestTime = 0;
    switch (algorithm) {

        case 'l': //l for LRTF 
            pthread_mutex_lock(&readyMutex);
            if (!head) {
                poppedPcb = NULL;
            } else {
                curr = head;

                while(curr) {

                    if (curr->time_remaining > longestTime) {
                        longestTime = curr->time_remaining;
                    }
                    curr = curr->next;
                }
                curr = head;
                prevNode = head;
                while (curr) {
                    if (curr->time_remaining == longestTime) {
                        poppedPcb = curr;
                        if (curr == head) {
                            head = curr->next;
                        } else {
                            prevNode->next = curr->next;
                        }
                        break;
                    }
                    prevNode = curr;
                    curr = curr->next;
                }
            }
            pthread_mutex_unlock(&readyMutex);
            return poppedPcb;
            break;

        default:    //for both fifo queue and round robin
            pthread_mutex_lock(&readyMutex);
            poppedPcb = head;
            if (poppedPcb) {
                head = poppedPcb->next;
            }
            pthread_mutex_unlock(&readyMutex);
            return poppedPcb;
    }

}
/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
    pthread_mutex_lock(&readyMutex);
    while (!head) {
        pthread_cond_wait(&notIdle, &readyMutex);
    }
    pthread_mutex_unlock(&readyMutex);
    schedule(cpu_id);
}


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{
    pthread_mutex_lock(&current_mutex);
    pcb_t* pcb = current[cpu_id];
    pcb->state = PROCESS_READY;
    pthread_mutex_unlock(&current_mutex);
    push(pcb);
    schedule(cpu_id);
}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
    pthread_mutex_lock(&current_mutex);
    pcb_t* pcb = current[cpu_id];
    pcb->state = PROCESS_WAITING;
    pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
    pthread_mutex_lock(&current_mutex);
    pcb_t* pcb = current[cpu_id];
    pcb->state = PROCESS_TERMINATED;
    pthread_mutex_unlock(&current_mutex);
    schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is LRTF, wake_up() may need
 *      to preempt the CPU with lower remaining time left to allow it to
 *      execute the process which just woke up with higher reimaing time.
 * 	However, if any CPU is currently running idle,
* 	or all of the CPUs are running processes
 *      with a higher remaining time left than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
    unsigned int lowTime;
    int lowID;
    process->state = PROCESS_READY;
    push(process);

    if (algorithm == 'l') {
        pthread_mutex_lock(&current_mutex);
        lowID = -1;
        lowTime = 10;
        for (int i = 0; i < cpu_count; i++) {
            if (!current[i]) {
                lowID = -1;
                break;
            }
            if (current[i]->time_remaining < lowTime) {
                lowTime = current[i]->time_remaining;
                lowID = i;
            }
        }
        pthread_mutex_unlock(&current_mutex);
        if (lowTime < process->time_remaining && lowID != -1) {
            force_preempt(lowID);
        }
    }
}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -l and -r command-line parameters.
 */
int main(int argc, char *argv[])
{
    unsigned int cpu_count;

    /* Parse command-line arguments */
    if (argc < 1 || argc > 4)
    {
        fprintf(stderr, "ECE 3056 OS Sim -- Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -l | -r <time slice> ]\n"
            "    Default : FIFO Scheduler\n"
	    "         -l : Longest Remaining Time First Scheduler\n"
            "         -r : Round-Robin Scheduler\n\n");
        return -1;
    }
    cpu_count = strtoul(argv[1], NULL, 0);
    timeSlice = -1;

    for (int i = 0; i < argc; i++) {
        if(strcmp(argv[i],"-r") == 0) {
            algorithm = 'r';
            timeSlice = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-l") == 0) {
            algorithm = 'l';
        } else {
            algorithm = 'f';
        }
    }

    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);

    head = NULL;
    pthread_mutex_init(&readyMutex, NULL);
    pthread_cond_init(&notIdle, NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}


