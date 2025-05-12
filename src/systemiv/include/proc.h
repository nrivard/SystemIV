#pragma once

#include <stdint.h>

#include "params.h"

typedef enum {
    PROC_UNUSED = 0,
    PROC_INIT,          // intermediate step during creation
    PROC_SLEEPING,      // 
    PROC_RUNNABLE,      // 
    PROC_ACTIVE,        // actively running
    PROC_ZOMBIE         // intermediate step during destruction
} proc_status_t;

typedef struct proc_ctx {
    uint32_t d2;
    uint32_t d3;
    uint32_t d4;
    uint32_t d5;
    uint32_t d6;
    uint32_t d7;

    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;

    uint32_t sp;
    uint32_t pc;
} proc_ctx_t;

typedef struct proc {
    int pid;
    char priority;
    char signal;
    struct proc *parent;
    proc_status_t status;
    struct proc_ctx context;
    char name[16];
    char *kstack;            // bottom of kernel stack for this process
    // TODO: current dir
    // TODO: open files
} proc_t;

extern proc_t procs[NPROC];

// creates the first process, the one to rule them all
void proc_bootstrap(void);

// initializes a new process
proc_t *proc_init(void);

// schedules the next process
void proc_sched(void);
