#include <ctype.h>

#include "proc.h"

#include "kalloc.h"
#include "printf.h"
#include "string.h"

proc_t procs[NPROC];

static proc_t *initproc;
static int pid_next = 1;

void proc_bootstrap() {
    memset(&procs, 0, sizeof(procs));

    proc_t *p = proc_init();
    if (!p) {
        panic("Couldn't bootstrap init process");
    }

    initproc = p;
    strcpy(p->name, "init.d");
    p->status = PROC_RUNNABLE;
}

// initializes a process
// returns proc_t pointer if successful, NULL if not
proc_t * proc_init() {
    proc_t *proc = NULL;
    
    for (int i = 0; i < NPROC; i++) {
        if (procs[i].status == PROC_UNUSED) {
            proc = &procs[i];
            break;
        }
    }

    if (proc && 
        ((proc->kstack = kalloc()) != NULL)
    ) {
        proc->status = PROC_INIT;
        proc->pid = pid_next++;

        memset(&proc->context, 0, sizeof(proc->context));
        proc->context.sp = (uint32_t)proc->kstack + PGSIZE;

        // // TODO: we need to copy values from the currently running process...
        // proc->parent = &procs[parent_id];
    }

    return  proc;
}

void proc_sched() {
    for (int i = 0; i < NPROC; i++) {
        
    }
}
