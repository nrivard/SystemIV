#include <ctype.h>
#include <device.h>
#include <params.h>
#include <string.h>

device_t devices[NDEV];

oserr_t device_add(device_t *device, unsigned int *idx) {
    device_t *dest = NULL;

    if (device->blk_size == 0) {
        return ERR_DEV_BLK_SZ;
    }

    int i;
    for (i = 0; i < NDEV; i++) {
        // is slot free?
        if (devices[i].blk_size) {
            continue;
        }

        dest = &devices[i];
    }

    if (!dest) {
        return ERR_DEV_FULL;
    }

    memcpy(dest, device, sizeof(device_t));
    if (idx) {
        *idx = i;
    }

    return NOERR;
}

oserr_t device_remove(unsigned int idx) {
    if (idx >= NDEV) {
        return ERR_IDX_OUT_OF_BOUNDS;
    }

    memset(&devices[idx], 0, sizeof(device_t));

    return NOERR;
}

oserr_t device_get(unsigned int idx, device_t *device) {
    if (idx >= NDEV) {
        return ERR_IDX_OUT_OF_BOUNDS;
    }

    if (devices[idx].blk_size == 0) {
        return ERR_DEV_NOT_FOUND;
    }

    memcpy(device, &devices[idx], sizeof(device_t));

    return NOERR;
}
