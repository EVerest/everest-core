#pragma once

#include "cb_platform.h"
#include <stdint.h>

typedef enum _CB_BaudRate {
    CB_B0 = 0000000, /* hang up */
    CB_B50 = 0000001,
    CB_B75 = 0000002,
    CB_B110 = 0000003,
    CB_B134 = 0000004,
    CB_B150 = 0000005,
    CB_B200 = 0000006,
    CB_B300 = 0000007,
    CB_B600 = 0000010,
    CB_B1200 = 0000011,
    CB_B1800 = 0000012,
    CB_B2400 = 0000013,
    CB_B4800 = 0000014,
    CB_B9600 = 0000015,
    CB_B19200 = 0000016,
    CB_B38400 = 0000017,
    CB_B57600 = 0010001,
    CB_B115200 = 0010002,
    CB_B230400 = 0010003,
    CB_B460800 = 0010004,
    CB_B500000 = 0010005,
    CB_B576000 = 0010006,
    CB_B921600 = 0010007,
    CB_B1000000 = 0010010,
    CB_B1152000 = 0010011,
    CB_B1500000 = 0010012,
    CB_B2000000 = 0010013,
    CB_B2500000 = 0010014,
    CB_B3000000 = 0010015,
    CB_B3500000 = 0010016,
    CB_B4000000 = 0010017,
} CB_BaudRate;

struct CB_COMPILER_ATTR_PACK cb_serial_message {
    uint8_t ixon;
    uint8_t parity;
    uint8_t cstopb;
    uint32_t cbaud;
    uint16_t data_size;
};

#include "test/cb_serial_message_test.h"
