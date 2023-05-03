// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <utils/serial.hpp>

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <unistd.h>

namespace Everest {
Serial::Serial() {
    fd = 0;
    baud = 0;
    cobsDecodeReset();
}

Serial::~Serial() {
    if (fd) {
        close(fd);
    }
}

bool Serial::openDevice(const char* device, int _baud) {

    fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Serial: error %d opening %s: %s\n", errno, device, strerror(errno));
        return false;
    } // else printf ("Serial: opened %s as %i\n", device, fd);
    cobsDecodeReset();

    switch (_baud) {
    case 9600:
        baud = B9600;
        break;
    case 19200:
        baud = B19200;
        break;
    case 38400:
        baud = B38400;
        break;
    case 57600:
        baud = B57600;
        break;
    case 115200:
        baud = B115200;
        break;
    case 230400:
        baud = B230400;
        break;
    default:
        baud = 0;
        return false;
    }

    return setSerialAttributes();
}

bool Serial::setSerialAttributes() {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        printf("Serial: error %d from tcgetattr\n", errno);
        return false;
    }

    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY);
    tty.c_lflag = 0;     // no signaling chars, no echo,
                         // no canonical processing
    tty.c_oflag = 0;     // no remapping, no delays
    tty.c_cc[VMIN] = 0;  // read blocks
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

    tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                       // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Serial: error %d from tcsetattr\n", errno);
        return false;
    }
    // printf ("Success setting tcsetattr\n");
    return true;
}

void Serial::cobsDecodeReset() {
    code = 0xff;
    block = 0;
    decode = msg;
}

uint32_t Serial::crc32(uint8_t* buf, int len) {
    int i, j;
    uint32_t b, crc, msk;

    i = 0;
    crc = 0xFFFFFFFF;
    while (i < len) {
        b = buf[i];
        crc = crc ^ b;
        for (j = 7; j >= 0; j--) {
            msk = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & msk);
        }
        i = i + 1;
    }
    // printf("%X",crc);
    return crc;
}

void Serial::handlePacket(uint8_t* buf, int len) {
    // FIXME (aw): why is there no implementation here?
}

void Serial::cobsDecode(uint8_t* buf, int len) {
    for (int i = 0; i < len; i++)
        cobsDecodeByte(buf[i]);
}

void Serial::cobsDecodeByte(uint8_t byte) {
    // check max length
    if ((decode - msg == 2048 - 1) && byte != 0x00) {
        printf("cobsDecode: Buffer overflow\n");
        cobsDecodeReset();
    }

    if (block) {
        // we're currently decoding and should not get a 0
        if (byte == 0x00) {
            // probably found some garbage -> reset
            printf("cobsDecode: Garbage detected\n");
            cobsDecodeReset();
            return;
        }
        *decode++ = byte;
    } else {
        if (code != 0xff) {
            *decode++ = 0;
        }
        block = code = byte;
        if (code == 0x00) {
            // we're finished, reset everything and commit
            if (decode == msg) {
                // we received nothing, just a 0x00
                printf("cobsDecode: Received nothing\n");
            } else {
                // set back decode with one, as it gets post-incremented
                handlePacket(msg, decode - 1 - msg);
            }
            cobsDecodeReset();
            return; // need to return here, because of block--
        }
    }
    block--;
}

} // namespace Everest
