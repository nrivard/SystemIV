#include "spinlock.h"
#include "m68k.h"

void spinlock_init(spinlock_t *lock, char *name) {
    lock->locked = 0;
    lock->name = name;
    m68k_wr_sr(m68k_rd_sr());
}
