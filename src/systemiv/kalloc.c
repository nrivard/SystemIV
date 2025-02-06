#include <stdint.h>
#include <ctype.h>

#include "kalloc.h"
#include "printf.h"

extern uint8_t _end[];
extern uint8_t _phys_end[];

typedef struct ll {
    struct ll *next;
} linked_list_t;

struct {
    linked_list_t *freelist;
} kmem;

void kinit() {
    // first page has NULL as `next`, which signals no more pages
    kmem.freelist = NULL;
    // now build initial freelist up one page at a time. 
    for (uint8_t *pg = (uint8_t *)PGROUNDUP((uint32_t)_end); pg + PGSIZE < _phys_end; pg += PGSIZE) {
        kfree(pg);
    }
}

void kfree(void *pg) {
    if ((uint32_t)pg % PGSIZE != 0 || pg < _end || pg > _phys_end) {
        panic("Out of bounds kfree!");
    }

    linked_list_t *prev = (linked_list_t *)pg;
    prev->next = kmem.freelist;
    kmem.freelist = prev;
}

void *kalloc() {
    linked_list_t *new;
    new = kmem.freelist;
    if (new) {
        kmem.freelist = new->next;
    }

    return (void *)new;
}
