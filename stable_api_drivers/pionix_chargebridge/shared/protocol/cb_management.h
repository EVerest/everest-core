// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

/**
 * \file Common structs used between linux and the STM32 dev board. Data
 * will be sent raw over the UDP socket, with the sender using
 * \ref send(struct, sizeof(CbStruct))
 * and the receiver using
 * \ref receive(raw_bytes, sizeof(CbStruct))
 * CbStruct *struct = reinterpret_cast<CbStruct *>(raw_bytes);
 *
 * Notes:
 * 1) After V1 structs will not be able to remove fields, only add fields
 *    after the existing fields
 * 2) There can be problems with variable length structs, for example
 *    CbFirmwarePacket that can have a payload with lengths 0-1024
 *
 * Test files are added at the end that check the sizes of the structs
 * at compile time, to determine the fact that they are consistent
 * across platforms.
 */
#pragma once

#include <stdint.h>

#include "cb_config.h"

#ifndef __cplusplus
#error "This header is C++ only"
#endif

enum class AppUDPResponse : uint32_t {
	AUR_Ok = 0x500D500D, AUR_Bad = 0xBADBAD00,
};

enum class CbType : uint8_t {
	CCS_EVSE = 0, CCS_EV = 1,
};

/*
 * What type of message is on the socket
 */
enum class CbStructType : uint16_t {

	// track IP with timeout and port
	// Housekeepig, heartbeat/config (safety_config, serial port(fixed)/CAN bitrate, gpio config, mdns module name), fw version, softreset,
	CST_HostToCb_Heartbeat = 1,
	CST_CbToHost_Heartbeat = 2,

	// track IP with timeout and port
	// GPIO client
	CST_HostToCb_Gpio = 3,
	CST_CbToHost_Gpio = 4,

	// FW update
	CST_CbFirmwareReply = 0xFFF9,
	CST_CbFirmwareStart = 0xFFFA,
	CST_CbFirmwarePacket = 0xFFFB,
	CST_CbFirmwareFinish = 0xFFFC,
	CST_CbFirmwareUpdateCancel = 0xFFFD,
	CST_CbFirmwarePing = 0xFFFE,
	CST_CbFirmwareGetVersion = 0xFFFF,
};


/*
 This container message is used for all generic module management packets
 */


template<typename T>
struct CB_COMPILER_ATTR_PACK CbManagementPacket {
	CbStructType type;
	T data;
};

template<> struct CB_COMPILER_ATTR_PACK CbManagementPacket<void> {
	CbStructType type;
};


struct CB_COMPILER_ATTR_PACK CbGpioPacket {
	uint8_t number_of_gpios; // Just to check compatibility
	uint16_t gpio_values[CB_NUMBER_OF_GPIOS]; // Actual value, 0: low, 1: high, or duty cycle for PWM
};

struct CB_COMPILER_ATTR_PACK CbHeartbeatPacket {
    CbConfig module_config;
};

struct CB_COMPILER_ATTR_PACK CbHeartbeatReplyPacket {
	int32_t cp_hi_mV;
	int32_t cp_lo_mV;
	int32_t vdd_core;
	int32_t vdd_3v3;
	int32_t vdd_refint;
	int32_t vdd_12V;
	int32_t vdd_N12V;
	int32_t pp_mOhm;
	int32_t pp_voltage_mV;
	uint8_t relay_state_feedback[3];
	int16_t temperature_mcu_C;
	int16_t temperature_pcb_C;
	int16_t temperature_modem_C;
	int16_t temperature_PT1000_C[2];
	int32_t uptime_ms;
};

struct CB_COMPILER_ATTR_PACK CbFirmwareStart {
	uint8_t is_secure_fw :1;
	uint8_t requires_crc_verification :1;
	uint8_t requires_sha256_verification :1;
	uint8_t requires_signature_verification :1;
	uint8_t requires_decryption :1;

	// IV in case we require decryption
	uint8_t iv[16];
};

struct CB_COMPILER_ATTR_PACK CbFirmwarePacket {
	// If it is the last packet sent, used when we need to take care of
	// the padding bytes in case of decryption or other operations
	uint8_t last_packet :1;

	uint16_t sector;
	uint16_t data_len;
	uint8_t data[1024];
};

struct CB_COMPILER_ATTR_PACK CbFirmwareEnd {
	uint32_t firmware_len;

	// Signature for the firmware in the faw format
	uint8_t fw_signature[128];
	uint8_t fw_signature_len;
	uint8_t watermark_secure_end;
};

struct CB_COMPILER_ATTR_PACK CbFirmwareUpdateCancel {
	uint8_t dummy;
};

struct CB_COMPILER_ATTR_PACK CbFirmwarePing {
	uint8_t dummy;
};

struct CB_COMPILER_ATTR_PACK CbFirmwareGetVersion {
	uint8_t dummy;
};

#include "test/cb_management_test.h"
