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

typedef struct {
    char data[0x80];
    uint8_t expected;
    uint8_t checksum;
} XModemPacket;

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

int xmodem_recv_packet(XModemPacket *packet) {

}

int xmodem_finish_tx() {

}

int xmodem_recv(const char *destination, int maxsize) {
    XModemPacket packet;
    packet.expected = 1;

    serial_put_string("Waiting for XModem transfer...\r\nPress ESC to cancel\r\n");

    // send NACK until we get START_OF_HEADER
    if(!xmodem_start_tx()) {
        return XMODEM_TIMEOUT;
    }

    int ret = 0;
    while (!ret) {
        uint8_t recvd = serial_get();

        switch (recvd) {
            case SOH:
                xmodem_recv_packet(&packet);
                break;
            case EOT:
                ret = xmodem_finish_tx();
                break;
            case ESC:
                // user cancelled
                ret = -1;
                break;
        }
    }

    return ret;
}
