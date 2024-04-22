#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "xmodem.h"
#include "serial.h"

#define SOH     0x01
#define EOT     0x04
#define ACK     0x06
#define NAK     0x15
#define ETB     0x17
#define CAN     0x18
#define ESC     0x1B

// timeout duration in seconds
#define XMODEM_TIMEOUT          3 * CLOCKS_PER_SEC
#define XMODEM_PACKET_SIZE      0x80

typedef struct {
    uint8_t data[XMODEM_PACKET_SIZE];
    uint8_t block;
    uint8_t block_complement;
    uint8_t calc_checksum;
    uint8_t checksum;
} xmodem_packet_t;

// initiates xmodem transfer and waits for data to be avail
// returns zero if no byte received, non-zero otherwise
int xmodem_start_tx() {
    int byte_avail = 0;
    while (!byte_avail) {
        // send NAK
        serial_put(NAK);

        clock_t start = clock();
        while ((byte_avail = serial_byte_avail()) == 0) {
            clock_t time = clock();
            if ((time - start) > XMODEM_TIMEOUT) {
                break;
            }
        }
    }

    return byte_avail;
}

void xmodem_recv_packet(xmodem_packet_t *packet) {
    packet->block = serial_get();
    packet->block_complement = serial_get();

    register uint8_t calc_checksum = 0;
    for (int i = 0; i < XMODEM_PACKET_SIZE; i++) {
        packet->data[i] = serial_get();
        calc_checksum += packet->data[i];
    }

    packet->checksum = serial_get();
    packet->calc_checksum = calc_checksum;
}

bool xmodem_packet_valid(uint8_t expected_block, xmodem_packet_t *packet) {
    return (
        packet->calc_checksum == packet->checksum &&
        packet->block == expected_block &&
        (packet->block + packet->block_complement) == 0xFF
    );
}

// returns 0 on success, otherwise an error code
int xmodem_recv(uint8_t *destination, int maxsize) {
    static xmodem_packet_t packet;

    // send NACK until we get START_OF_HEADER
    if(!xmodem_start_tx()) {
        return XMODEM_TIMEOUT;
    }

    int ret = 0;
    uint8_t expected = 1;
    while (!ret) {
        uint8_t recvd = serial_get();

        switch (recvd) {
            case SOH:
                xmodem_recv_packet(&packet);

                if (!xmodem_packet_valid(expected, &packet)) {
                    // invalid packet
                    serial_put(NAK);
                    continue;
                }

                // write packet data to dest...
                for (int i = 0; i < XMODEM_PACKET_SIZE; i++) {
                    *destination++ = packet.data[i];
                }

                // success
                serial_put(ACK);

                expected++;

                break;

            case EOT:
                serial_put(ACK);
                return 0;

            case ESC:
            default:
                return recvd;
        }
    }

    // shouldn't ever get here
    return -1;
}
