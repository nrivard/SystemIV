#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "time.h"
#include "shellac.h"
#include "serial.h"

extern int dumb();

#define PARAM_SIZE 10

typedef struct {
    // up to 10 params: name, addr, 8 additional values
    // ex: write 1, 2, 3, 4 starting at address $001000
    //      wr 001000 01 02 03 04
    char *params[PARAM_SIZE];
    int numParams;
} Input;

typedef struct {
    char *name;
    char *usage;
    bool (*function)(char *args[], int count);
} Command;

bool command_read(char *args[], int count);
bool command_write(char *args[], int count);
bool command_transfer(char *args[], int count);
bool command_execute(char *args[], int count);

static Command commands[] = {
    {"rd", "rd <addr> (<length> = 1)", &command_read},
    {"wr", "wr <addr> <value1> (<value2>...<value8>)", &command_write},
    {"tx", "tx <addr>", &command_transfer},
    {"ex", "ex <addr>", &command_execute}
};

char *command_read_input(char *buffer, int size) {
    int recvd = 0;
    char *ptr = buffer;
    while (recvd < size - 1) {
        *ptr = serial_get();

        // echo received char
        serial_put(*ptr); 

        if (*ptr == '\n') {
            break;
        }

        ptr++;
        recvd++;
    }
    return buffer;
}

void command_tokenize(char *buffer, Input *input) {
    input->numParams = 0;
    while (input->numParams < PARAM_SIZE) {
        input->params[input->numParams++] = buffer;
        while (1) {
            char *dest = buffer++;
            if (*dest == ' ') {
                *dest = '\0';
                break;
            } else if (*dest == '\r' || *dest == '\n') {
                *dest = '\0';
                return;
            }
        }
    }
}

 bool command_parse(Input *input, Command *command) {
    char *cmdName = input->params[0];
    int size = sizeof(commands) / sizeof(Command);
    for (int i = 0; i < size; i++) {
        Command curr = commands[i];
        if (strcmp(curr.name, cmdName) == 0) {
            *command = curr;
            return true;
        }
    }
    return false;
}

bool command_read(char *args[], int count) {
    if (count < 1) {
        return false;
    }

    uint8_t *addr = (uint8_t *)strtoul(args[0], NULL, 16);
    unsigned long len = count > 1 ? strtoul(args[1], NULL, 16) : 1;

    // // printf("Reading %d bytes from %p", len, addr);
    for (int i = 0; i < len; i++) {
        if ((i % 8) == 0) {
            serial_put_string("\r\n");
            serial_put_long((unsigned long)&addr[i]);
            serial_put_string(":  ");
        }

        serial_put_hex(addr[i] & 0xFF);
        serial_put(' ');
    }

    return true;
}

bool command_write(char *args[], int count) {
    if (count < 2 || count > 9) {
        return false;
    }

    uint8_t *addr = (uint8_t *)strtoul(args++[0], NULL, 16);

    // // printf("Writing %d bytes to %p", count - 1, addr);
    for (int i = 0; i < count - 1; i++) {
        *addr++ = 0xFF & strtoul(args[i], NULL, 16);
    }

    return true;
}

bool command_transfer(char *args[], int count) {
    clock_t start = clock();
    serial_put_long(start);

    while (!serial_byte_avail());

    clock_t elapsed = clock() - start;
    serial_put_long(elapsed);

    return true;
}

bool command_execute(char *args[], int count) {
    if (count < 1 || (strlen(args[0]) == 0)) {
        return false;
    }

    int (*function)(void) = (int (*)(void))strtoul(args[0], NULL, 16);

    return function();
}

void shellac_main() {
    char buffer[0x80];
    Input input;
    Command command;

    serial_put_string("Shellac v1.0\r\n");

    for(;;) {
        serial_put('>');
        if (!command_read_input(buffer, sizeof(buffer))) {
            serial_put_string("FATAL: couldn't read user input!\r\n");
            return;
        }

        command_tokenize(buffer, &input);

        if (!command_parse(&input, &command)) {
            serial_put_string("Invalid command: \"");
            serial_put_string(input.params[0]);
            serial_put_string("\"\r\n");
            continue;
        }

        if (!command.function(&input.params[1], input.numParams - 1)) {
            serial_put_string("usage: ");
            serial_put_string(command.usage);
        }

        serial_put_string("\r\n");
    }
}
