#include <stdio.h>
#include <getopt.h>

#include "pagesim.h"
#include "paging.h"
#include "swap.h"
#include "stats.h"
#include "swapops.h"

/* Simulator data structures */
uint8_t *mem;
pfn_t PTBR;
pcb_t *current_process;
uint8_t check_corruption = 0;
uint8_t replacement = 0;

/* Internal array of running processes (we only expose current_process
   to the user) */
static pcb_t *procs;

/* Constants used in parsing the trace file */
static const char *START = "START";
static const char *STOP = "STOP";

void print_help_and_exit(void);
void check_validity(int checks);

int main(int argc, char **argv)
{
    /* Allocate some memory! */
    if (!(mem = calloc(1, MEM_SIZE))) {
        exit(1);
    }

    /* Allocate procs */
    if (!(procs = calloc(MAX_PID, sizeof(pcb_t)))) {
        exit(1);
    }

    /* Read command line options */
    FILE *fin = 0;
    int opt;
    while (-1 != (opt = getopt(argc, argv, "i:hscr:"))) {
        switch (opt) {
        case 'i':
            fin = fopen(optarg, "r");

            if (!fin) {
                perror("Unable to open trace file");
                exit(1);
            }
            break;
        case 's':
            fin = stdin;
            break;
        case 'c':
            check_corruption = 1;
            printf("-> Note: Strict memory corruption checking is enabled.\n");
            break;
        case 'r':
            if (strcmp(optarg, "random") == 0) {
                replacement = RANDOM;
            } else if (strcmp(optarg, "clocksweep") == 0) {
                replacement = CLOCKSWEEP;
            } else {
                fprintf(stderr, "Unknown replacement algorithm: %s", optarg);
                exit(1);
            }
            break;
        case 'h':
        default:
            /* Print some sort of usage message and exit */
            print_help_and_exit();
        }
    }

    if (!fin) {
        fprintf(stderr, "ERROR: You must specify a trace filename or stdin.\n");
        print_help_and_exit();
    }
    if (!replacement) {
	 replacement = RANDOM;
        // fprintf(stderr, "ERROR: You must select a replacement algorithm using -r.\n");
        // print_help_and_exit();
    }

    /* Start the simulation */
    char rw;
    uint8_t data;
    uint32_t address;
    uint32_t pid;
    char buf[120];
    uint32_t step = 0;

    system_init();
    if (check_corruption) check_validity(0);

    while ((fgets(buf, sizeof(buf), fin))) {
        /* Check if process is starting */
        if (!strncmp(buf, START, 5)) {
            /* Start scanning from the pid digits */
            int ret = sscanf(buf+6, "%" PRIu32 "\n", &pid);
            if (ret == 1) {
                /* Initialize new process */
                pcb_t *new_proc = &procs[pid];
                new_proc->pid = pid;
                new_proc->state = PROC_RUNNING;
                proc_init(new_proc);
                printf("%8u: PID %u started\n", step, pid);
                if (check_corruption) check_validity(1);
            } else {
                printf("Unable to parse trace file: Invalid START command encountered\n");
                exit(1);
            }
        } else if (!strncmp(buf, STOP, 4)) { /* Check if process is stopping */
            /* Start scanning from the pid digits */
            int ret = sscanf((buf+5), "%" PRIu32 "\n", &pid);
            if (ret == 1) {
                proc_cleanup(&procs[pid]);
                procs[pid].saved_ptbr = 0;
                procs[pid].state = PROC_STOPPED;
                printf("%8u: PID %u stopped\n", step, pid);
                if (check_corruption) check_validity(1);
            } else {
                printf("Unable to parse trace file: Invalid STOP command encountered\n");
                exit(1);
            }
        } else { /* Regular access trace */
            int ret = sscanf(buf, "%u %c %x %hhu\n", &pid, &rw, &address, &data);
            if (ret == 4) {
                /* Context switch if need be */
                if (!current_process || current_process->pid != pid) {
                    context_switch(&procs[pid]);
                    current_process = &procs[pid];
                }
                uint8_t new_data = mem_access(address, rw, data);
                /* Print data for trace verification */
                if (rw == 'r') {
                    printf("%8u: %3u  r  0x%05x -> %02hhx\n", step, pid, address, new_data);
                } else {
                    printf("%8u: %3u  w  0x%05x <- %02hhx\n", step, pid, address, data);
                }
                if (check_corruption) check_validity(1);
            } else {
                printf("Unable to parse trace file: Invalid memory access command encountered\n");
                exit(1);
            }
        }

        step++;                 /* Count step number for easy debugging */
    }
    fclose(fin);

    /* Cleanup and print statistics */
    free(mem);
    free(procs);
    compute_stats();

    printf("Total Accesses     : %" PRIu64 "\n", stats.accesses);
    printf("Reads              : %" PRIu64 "\n", stats.reads);
    printf("Writes             : %" PRIu64 "\n", stats.writes);
    printf("Page Faults        : %" PRIu64 "\n", stats.page_faults);
    printf("Writes to disk     : %" PRIu64 "\n", stats.writebacks);
    printf("Average Access Time: %f\n", stats.aat);
    printf("Max Swap Size      : %" PRIu64 " KB\n", (((uint64_t) swap_queue.size_max) * PAGE_SIZE) >> 10);

    if (swap_queue.size > 0)  {
        printf("Swap Not Freed     : %" PRIu64 " KB\n", (((uint64_t) swap_queue.size) * PAGE_SIZE) >> 10);
    }
}

void check_validity(int checks) {
    uint32_t pid, vpn, pfn, running_procs;
    uint8_t protected_frames_accounted_for[NUM_FRAMES];
    uint8_t mapped_frames_accounted_for[NUM_FRAMES];
    for (pfn = 0; pfn < NUM_FRAMES; pfn++) {
        protected_frames_accounted_for[pfn] = 0;
        mapped_frames_accounted_for[pfn] = 0;
    }

    /* Validate frame table is set up correctly */
    if ((void *)frame_table != (void *) mem) {
        panic("Frame table should begin at the first frame in memory");
    }

    if (!frame_table[0].protected) {
        panic("Frame 0 should be marked as protected");
    }
    protected_frames_accounted_for[0] = 1;

    if (checks < 1) return;

    /* Validate the PTBRs are correct */
    running_procs = 0;
    for (pid = 0; pid < MAX_PID; pid++) {
        if (procs[pid].state == PROC_RUNNING) {
            running_procs++;

            /* Validate that PTBR points to a correct physical frame number */
            pfn_t found_ptbr = procs[pid].saved_ptbr;
            if (found_ptbr <= 0 || found_ptbr > NUM_FRAMES)  {
                panic("PTBR of running process cannot be zero or >= the number of frames in the system");
            }

            /* Validate that page table page is marked as protected */
            if (!frame_table[found_ptbr].protected) {
                panic("Frames corresponding to the page tables of running processes must be marked as protected");
            }
            protected_frames_accounted_for[found_ptbr] = 1;
        }
    }

    /* Check for any protected frames that should not be protected */
    for (pfn = 0; pfn < NUM_FRAMES; pfn++) {
        if (frame_table[pfn].protected && !protected_frames_accounted_for[pfn]) {
            panic("Found frame marked as protected that should not be protected");
        }
    }

    /* Validate the page table entries are correct */
    for (pid = 0; pid < MAX_PID; pid++) {
        if (procs[pid].state == PROC_RUNNING) {
            pfn_t found_ptbr = procs[pid].saved_ptbr;

            /* Scan the entire page table, make sure frame table is
               consistent with any valid pages */
            pte_t *pgtable = (pte_t *)(mem + (found_ptbr * PAGE_SIZE));
            for (vpn = 0; vpn < NUM_PAGES; vpn++) {
                /* Check basic sanity of boolean flags */
                if (pgtable[vpn].valid != 0 && pgtable[vpn].valid != 1) {
                    panic("Page table entry valid bit should either be zero or one");
                }

                if (pgtable[vpn].dirty != 0 && pgtable[vpn].dirty != 1) {
                    panic("Page table entry dirty bit should either be zero or one");
                }

                /* If valid, check sanity of pfn */
                if (pgtable[vpn].valid) {
                    pfn_t found_pfn = pgtable[vpn].pfn;

                    /* Check basic ranges */
                    if (found_pfn <= 0 || found_pfn > NUM_FRAMES - 1)  {
                        panic("PFN of page table entry cannot be zero or >= the number of frames in the system");
                    }

                    if (protected_frames_accounted_for[found_pfn]) {
                        panic("Page table entry should not map to a protected frame");
                    }

                    if (mapped_frames_accounted_for[found_pfn]) {
                        panic("Duplicate PFN found in page table");
                    }

                    if (frame_table[found_pfn].process < procs
                        || frame_table[found_pfn].process >= procs + MAX_PID) {
                        panic("Mapped frame table entry contains invalid process pointer");
                    }

                    /* Check that frame table agrees with page table */
                    if (!frame_table[found_pfn].mapped
                        || !(frame_table[found_pfn].process->pid == pid)
                        || !(frame_table[found_pfn].vpn == vpn)) {
                        panic("Frame table is inconsistent with page table entry");
                    }
                    mapped_frames_accounted_for[found_pfn] = 1;
                }

                /* Check the validity of swap entry */
                if (pgtable[vpn].swap && !swap_queue_find(&swap_queue, pgtable[vpn].swap)) {
                    panic("Page table entry points to swap entry that does not exist");
                }
            }
        }
    }

    /* Check that all frames that are mapped are accounted for */
    for (pfn = 0; pfn < NUM_FRAMES; pfn++){
        if (!frame_table[pfn].protected && frame_table[pfn].mapped && !(mapped_frames_accounted_for[pfn])) {
            panic("Found frame table entry marked as mapped with no corresponding page table entry");
        }
    }
}

void print_help_and_exit() {
    printf("./vm-sim [OPTIONS] -i traces/file.trace -r<replacement algorithm>\n");
    printf("  -i\t\tReads the trace from the specified path\n");
    printf("  -s\t\tReads the trace from standard input\n");
    printf("  -r\t\tSelect the replacement algorithm (either 'random' or 'clocksweep')\n");
    printf("  -c\t\tEnables strict memory corruption checking\n");
    printf("    \t\t(automatically checks a variety of conditions that can cause bugs)\n");
    printf("  -h\t\tThis helpful output\n");
    exit(0);
}
