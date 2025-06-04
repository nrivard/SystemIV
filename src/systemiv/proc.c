#include <ctype.h>

#include "proc.h"

#include "m68k.h"
#include "kalloc.h"
#include "printf.h"
#include "string.h"

extern uint8_t _proc_start[];

proc_t procs[NPROC];

static proc_t *initproc;
static unsigned int curr;
static int pid_next = 1;

void proc_bootstrap() {
    memset(&procs, 0, sizeof(procs));

    proc_t *p = proc_init();
    if (!p) {
        panic("Couldn't bootstrap init process");
    }

    p->status = PROC_RUNNABLE;
    strncpy(p->name, "init.d", sizeof(p->name));

    initproc = p;
}

// initializes a process
// returns proc_t pointer if successful, NULL if not
proc_t * proc_init() {
    unsigned int lane;
    for (lane = 0; lane < NPROC; lane++) {
        if (procs[lane].status == PROC_UNUSED) {
            break;
        }
    }

    if (lane >= NPROC) {
        return NULL;
    }

    proc_t *proc = &procs[lane];
    proc_t *parent = proc_curr();

    proc->status = PROC_INIT;
    proc->pid = pid_next++;

    memset(&proc->context, 0, sizeof(proc->context));
    proc->parent = &procs[curr];
    proc->context.sp = (uint32_t)(_proc_start + (PROCSZ * (lane + 1))); // set stack to top of process's addr space

    // copy parent attr
    strlcpy(proc->name, parent->name, sizeof(proc->name));

    printf("proc %d: %l\n", lane, proc->context.sp);

    // // TODO: we need to copy values from the currently running process...

    return  proc;
}

proc_t *proc_sched() {
    unsigned int next = curr;

    for (int i = curr; i != curr; i = (i + 1) % NPROC) {
        if (procs[i].status == PROC_RUNNABLE) {
            next = i;
            break;
        }
    }

    // if next == curr, nothing to switch to!
    return (next == curr) ? NULL : &procs[next];
}

proc_t *proc_curr() {
    return &procs[curr];
}
