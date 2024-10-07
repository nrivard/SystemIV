#pragma once

typedef struct {
    unsigned int locked;

    char *name;
} spinlock_t;

void spinlock_init(spinlock_t *lock, char *name);
void spinlock_lock(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);
