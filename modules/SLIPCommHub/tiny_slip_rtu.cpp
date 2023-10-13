// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

// TODOs:
// - sometimes we receive 0 bytes from sofar, find out why
// - implement echo removal for chargebyte
// - implement GPIO to switch rx/tx

#include "tiny_slip_rtu.hpp"

#include <cstring>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <type_traits>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>

#include <fmt/core.h>

#include "crc16.hpp"

namespace tiny_slip {

// This is a replacement for system library tcdrain().
// tcdrain() returns when all bytes are written to the UART, but it actually returns about 10msecs or more after the
// last byte has been written. This function tries to return as fast as possible instead.
static void fast_tcdrain(int fd) {
    // in user space, the only way to find out if there are still bits to be shiftet out is to poll line status register
    // as fast as we can
    uint32_t lsr;
    do {
        ioctl(fd, TIOCSERGETLSR, &lsr);
    } while (!(lsr & TIOCSER_TEMT));
}

static auto check_for_exception(uint8_t received_function_code) {
    return received_function_code & (1 << 7);
}

static void clear_exception_bit(uint8_t& received_function_code) {
    received_function_code &= ~(1 << 7);
}

static std::string hexdump(const uint8_t* msg, int msg_len) {
    std::stringstream ss;
    for (int i = 0; i < msg_len; i++) {
        ss << std::hex << (int)msg[i] << " ";
    }
    return ss.str();
}

static void append_checksum(uint8_t* msg, int msg_len) {
/*    if (msg_len < 5)
        return;*/
    uint16_t crc_sum = calculate_modbus_crc16(msg, msg_len - 2);
    memcpy(msg + msg_len - 2, &crc_sum, 2);
}

static bool validate_checksum(const uint8_t* msg, int msg_len) {
    if (msg_len < 5)
        return false;
    // check crc
    uint16_t crc_sum = calculate_modbus_crc16(msg, msg_len - 2);
    uint16_t crc_msg;
    memcpy(&crc_msg, msg + msg_len - 2, 2);
    return (crc_msg == crc_sum);
}

static uint16_t num_of_slip_characters(uint8_t* msg, int msg_len) {
    uint16_t num = 0;
    for(int i=0; i<msg_len; i++){
        if(msg[i] == 0xC0 || msg[i] == 0xDB)
            num++;
    }
    return num;
}

static void add_slip_characters(uint8_t* msg_raw, int msg_raw_len, 
                                uint8_t* msg_slip, int msg_slip_len) {
    msg_slip[0] = 0xC0;
    if(msg_slip_len - msg_raw_len == 2) {
        memcpy(&msg_slip[1], msg_raw, msg_raw_len);
    }
    else {
        for(int i=0, j=1; i<msg_raw_len; i++) {   
            switch(msg_raw[i]){
                case 0xC0:
                    msg_slip[j++] = 0xDB;
                    msg_slip[j++] = 0xDC;
                    break;
                case 0xDB:
                    msg_slip[j++] = 0xDB;
                    msg_slip[j++] = 0xDD;
                    break;
                default:
                    msg_slip[j++] = msg_raw[i];
                    break;
            }
        }
    }
    msg_slip[msg_slip_len-1] = 0xC0;
}


static std::vector<uint16_t> decode_reply(const uint8_t* buf, int len, uint8_t expected_device_address,
                                          uint16_t expected_logical_address) {
    std::vector<uint16_t> result;
    if (len < SLIP_MIN_REPLY_SIZE) {
        EVLOG_error << fmt::format("Packet too small: {} bytes.", len);
        return result;
    }

    //ToDo:
    //SLIP characters entfernen

    /*if (expected_device_address != buf[DEVICE_ADDRESS_POS]) {
        EVLOG_error << fmt::format("Device address mismatch: expected: {} received: {}", expected_device_address,
                                   buf[DEVICE_ADDRESS_POS])
                    << ": " << hexdump(buf, len);

        return result;
    }*/
    
    bool exception = false;
    /*uint8_t function_code_recvd = buf[FUNCTION_CODE_POS];
    if (check_for_exception(function_code_recvd)) {
        // highest bit is set for exception reply
        exception = true;
        // clear error bit
        clear_exception_bit(function_code_recvd);
    }*/
    uint16_t register_address_recvd = ((uint16_t)buf[REGISTER_ADDR_HIGH_BYTE]) << 8 + buf[REGISTER_ADDR_LOW_BYTE];
    if (expected_logical_address != register_address_recvd) {
        EVLOG_error << fmt::format("Logical address mismatch: expected: {} received: {}",
                                   /*static_cast<std::underlying_type_t<FunctionCode>>*/(expected_logical_address), register_address_recvd);
        return result;
    }

    if (!validate_checksum(buf, len)) {
        EVLOG_error << "Checksum error";
        return result;
    }
    /*
    if (!exception) {
        // For a write reply we always get 4 bytes
        uint8_t byte_cnt = 4;
        int start_of_result = RES_TX_START_OF_PAYLOAD;

        // Was it a read reply?
        if (function == FunctionCode::READ_COILS || function == FunctionCode::READ_DISCRETE_INPUTS ||
            function == FunctionCode::READ_MULTIPLE_HOLDING_REGISTERS ||
            function == FunctionCode::READ_INPUT_REGISTERS) {
            // adapt byte count and starting pos
            byte_cnt = buf[RES_RX_LEN_POS];
            start_of_result = RES_RX_START_OF_PAYLOAD;
        }

        // check if result is completely in received data
        if (start_of_result + byte_cnt > len) {
            EVLOG_error << "Result data not completely in received message.";
            return result;
        }

        // ready to copy actual result data to output
        result.reserve(byte_cnt / 2);

        for (int i = start_of_result; i < start_of_result + byte_cnt; i += 2) {
            uint16_t t;
            memcpy(&t, buf + i, 2);
            t = be16toh(t);
            result.push_back(t);
        }
        return result;
    } else {
        // handle exception message
        uint8_t err_code = buf[RES_RX_START_OF_PAYLOAD];
        switch (err_code) {
        case 0x01:
            EVLOG_error << "Modbus exception: Illegal function";
            break;
        case 0x02:
            EVLOG_error << "Modbus exception: Illegal data address";
            break;
        case 0x03:
            EVLOG_error << "Modbus exception: Illegal data value";
            break;
        case 0x04:
            EVLOG_error << "Modbus exception: Client device failure";
            break;
        case 0x05:
            EVLOG_debug << "Modbus ACK";
            break;
        case 0x06:
            EVLOG_error << "Modbus exception: Client device busy";
            break;
        case 0x07:
            EVLOG_error << "Modbus exception: NACK";
            break;
        case 0x08:
            EVLOG_error << "Modbus exception: Memory parity error";
            break;
        case 0x09:
            EVLOG_error << "Modbus exception: Out of resources";
            break;
        case 0x0A:
            EVLOG_error << "Modbus exception: Gateway path unavailable";
            break;
        case 0x0B:
            EVLOG_error << "Modbus exception: Gateway target device failed to respond";
            break;
        }
        return result;
    }*/
    return result;
}

TinySlipRTU::~TinySlipRTU() {
    if (fd)
        close(fd);
}

bool TinySlipRTU::open_device(const std::string& device, int _baud, bool _ignore_echo,
                              const Everest::GpioSettings& rxtx_gpio_settings, const Parity parity) {

    ignore_echo = _ignore_echo;

    rxtx_gpio.open(rxtx_gpio_settings);
    rxtx_gpio.set_output(true);

    fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        EVLOG_error << fmt::format("Serial: error {} opening {}: {}\n", errno, device, strerror(errno));
        return false;
    }

    int baud;
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
        return false;
    }

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
    tty.c_lflag = 0;                      // no signaling chars, no echo,
                                          // no canonical processing
    tty.c_oflag = 0;                      // no remapping, no delays
    tty.c_cc[VMIN] = 1;                   // read blocks
    tty.c_cc[VTIME] = 1;                  // 0.1 seconds inter character read timeout after first byte was received

    tty.c_cflag |= (CLOCAL | CREAD);      // ignore modem controls,
                                          // enable reading
    if (parity == Parity::ODD) {
        tty.c_cflag |= (PARENB | PARODD); // odd parity
    } else if (parity == Parity::EVEN) {  // even parity
        tty.c_cflag &= ~PARODD;
        tty.c_cflag |= PARENB;
    } else {
        tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    }
    tty.c_cflag &= ~CSTOPB;                // 1 Stop bit
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Serial: error %d from tcsetattr\n", errno);
        return false;
    }
    return true;
}

int TinySlipRTU::read_reply(uint8_t* rxbuf, int rxbuf_len) {
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = SLIP_RX_INITIAL_TIMEOUT_MS * 1000; // 500msec intial timeout until device responds
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);

    int bytes_read_total = 0;
    while (true) {
        int rv = select(fd + 1, &set, NULL, NULL, &timeout);
        timeout.tv_usec = SLIP_RX_WITHIN_MESSAGE_TIMEOUT_MS *
                          1000; // reduce timeout after first chunk, no uneccesary waiting at the end of the message
        if (rv == -1) {         // error in select function call
            perror("txrx: select:");
            break;
        } else if (rv == 0) { // no more bytes to read within timeout, so transfer is complete
            break;
        } else {              // received more bytes, add them to buffer
            // do we have space in the rx buffer left?
            if (bytes_read_total >= rxbuf_len) {
                // no buffer space left, but more to read.
                break;
            }

            int bytes_read = read(fd, rxbuf + bytes_read_total, rxbuf_len - bytes_read_total);
            if (bytes_read > 0) {
                bytes_read_total += bytes_read;
                // EVLOG_info << "RECVD: " << hexdump(rxbuf, bytes_read_total);
            }
        }
    }
    return bytes_read_total;
}

/*
    This function transmits a SLIP request and waits for the reply.

*/
std::vector<uint16_t> TinySlipRTU::txrx(uint8_t device_address, uint16_t logical_address,
                                        uint8_t status_code, 
                                        std::vector<uint8_t> request,
                                        bool wait_for_reply) {
    //calculating frame len 
    //(containing logical address(2), length field(2), status code(1) and request data)
    //excluding device address, CRC and SLIP START+END ID 
    uint16_t app_layer_frame_length = request.size() + 5;
    
    // size of request (including device address, CRC and application data)
    int req_len = request.size() + SLIP_LINK_LAYER_OVERHEAD_SIZE;
    std::unique_ptr<uint8_t[]> req(new uint8_t[req_len]);

    // add header
    req[DEVICE_ADDRESS_POS] = device_address;
    logical_address = htobe16(logical_address);
    memcpy(req.get() + REGISTER_ADDR_POS, &logical_address, 2);   
    app_layer_frame_length = htobe16(app_layer_frame_length);
    memcpy(req.get() + FRAME_LEN_POS, &app_layer_frame_length, 2);

    //copy passed vector data
    int i = START_OF_PAYLOAD;
    for (auto r : request) {
        memcpy(req.get() + i, &r, 1);
        i++;
    }
            
    // set checksum (excluding the SLIP START+END ID)
    append_checksum(req.get(), req_len);      

    EVLOG_info << "SEND: " << hexdump(req.get(), req_len);

    //test if frame contains SLIP special characters
    //zuerst Anzahl der special characters im (END(0xC0) und ESC(0xDB))
    //Frame ermitteln 
    //--> neuen Puffer mit neuer Länge anlegen
    //(alte Länge + 2 (START+ENDE) + (Anzahl der special character)*2)
    //-->Frame umkopieren (falls keine special characters vorkommen, großer memcpy)
    //andernfalls byteweise, mit START+END
    //--> Frame senden
    uint16_t num = num_of_slip_characters(req.get(), req_len);
    int slipreq_len = req_len + 2 + num*2;
    std::unique_ptr<uint8_t[]> slipreq(new uint8_t[slipreq_len]);
    add_slip_characters(req.get(), req_len, slipreq.get(), slipreq_len);

    // clear input and output buffer
    tcflush(fd, TCIOFLUSH);

    // write to serial port
    rxtx_gpio.set(false);
    write(fd, slipreq.get(), slipreq_len);
    if (rxtx_gpio.is_ready()) {
        // if we are using GPIO to switch between RX/TX, use the fast version of tcdrain with exact timing
        fast_tcdrain(fd);
    } else {
        // without GPIO switching, use regular tcdrain as not all UART drivers implement the ioctl
        tcdrain(fd);
    }
    rxtx_gpio.set(true);

    if (ignore_echo) {
        // read back echo of what we sent and ignore it
        read_reply(slipreq.get(), slipreq_len);
    }

    if(wait_for_reply) {
        // wait for reply
        uint8_t rxbuf[SLIP_MAX_REPLY_SIZE];
        int bytes_read_total = read_reply(rxbuf, sizeof(rxbuf));
        return decode_reply(rxbuf, bytes_read_total, device_address, logical_address);
    }
    return std::vector<uint16_t>();
}

} // namespace tiny_slip
