#pragma once

#include "cb_platform.h"
#include <stdint.h>

#define CB_NUMBER_OF_GPIOS 10
#define CB_NUMBER_OF_UARTS 3

// enums

typedef enum _CbGpioMode : uint8_t {
	CBG_Input = 0x00,
	CBG_Output = 0x01,
	CBG_Pwm_Input = 0x02,
	CBG_Pwm_Output = 0x03,
	CBG_RS485_2_DE = 0x04,
	CBG_Rcd_Selftest_Output = 0x05,
	CBG_Rcd_Error_Input= 0x06,
	CBG_Rcd_PWM_Input= 0x07,
	CBG_MotorLock_1 = 0x08,
	CBG_MotorLock_2 = 0x09,
} CbGpioMode;

typedef enum _CbRelayMode : uint8_t {
	CBR_PowerRelay = 0x00, CBR_UserRelay = 0x01,
} CbRelayMode;

typedef enum _CbGpioPulls : uint8_t {
	CBGP_NoPull = 0x00, CBGP_PullUp = 0x01, CBGP_PullDown = 0x02,
} CbGpioPulls;

typedef enum _CbUartBaudrate : uint8_t {
	CBUBR_9600 = 0x00,
	CBUBR_19200 = 0x01,
	CBUBR_38400 = 0x02,
	CBUBR_57600 = 0x03,
	CBUBR_115200 = 0x04,
	CBUBR_230400 = 0x05,
	CBUBR_250000 = 0x06,
	CBUBR_460800 = 0x07,
	CBUBR_500000 = 0x08,
	CBUBR_1000000 = 0x09,
	CBUBR_2000000 = 0x0A,
	CBUBR_3000000 = 0x0B,
	CBUBR_4000000 = 0x0C,
	CBUBR_6000000 = 0x0D,
	CBUBR_8000000 = 0x0E,
	CBUBR_10000000 = 0x0F,
} CbUartBaudrate;

typedef enum _CbUartStopbits : uint8_t {
	CBUS_OneStopBit = 0x00, CBUS_TwoStopBits = 0x01,
} CbUartStopbits;

typedef enum _CbUartParity : uint8_t {
	CBUP_None = 0x00, CBUP_Odd = 0x01, CBUP_Even = 0x02,
} CbUartParity;

typedef enum _CbCanBaudrate : uint8_t {
	CBCBR_125000 = 0x00,
	CBCBR_250000 = 0x01,
	CBCBR_500000 = 0x02,
	CBCBR_1000000 = 0x03,
} CbCanBaudrate;

typedef enum _CbSafetyMode : uint8_t {
	CBSM_disabled = 0x00, CBSM_US = 0x01, CBSM_EU = 0x02,

} CbSafetyMode;

// Structs

typedef struct CB_COMPILER_ATTR_PACK _relay_config {
	CbRelayMode relay_mode;
	uint8_t feedback_enabled;    // 0: feedback unused, 1: feedback expected
	uint16_t feedback_delay_ms; // After switching, wait for this delay before evaluating feedback pin
	uint8_t feedback_inverted; // 0: feedback normal (mirror contact, high when relay is off), 1: inverted
	uint8_t pwm_dc; // 100: Do not use PWM. 1-99: Set PWM Duty cycle after delay
	uint16_t pwm_delay_ms;       // Delay in ms after which the PWM starts
	uint16_t switchoff_delay_ms; // Delay before switching relay off. Can be used to set a small delay between EMG_OUT
								 // and relays off [SR-SL-2]
} RelayConfig;

typedef struct CB_COMPILER_ATTR_PACK _safety_config {
	CbSafetyMode pp_mode;       // set to 0: disabled 1: US type 1, 2: EU type 2
	uint8_t cp_avg_ms;     // default is 10ms / pulses
	RelayConfig relays[3]; // Config for the 3 relay I/Os
	uint8_t inverted_emergency_input; // 0: normal operation, 1: emergency input is inverted
	uint8_t temperature_limit_pt1000_C; // Temperature limit for the PT1000 inputs. Relays will switch off if temperature is above the limit.
} SafetyConfig;

typedef struct CB_COMPILER_ATTR_PACK _CbGpioConfig {
	CbGpioMode mode;
	CbGpioPulls pulls;
	uint8_t strap_option_mdns_naming; // sample as bit for mdns id;
	uint16_t mode_config;    // Config value for the mode, e.g. frequency of PWM
} CbGpioConfig;

typedef struct CB_COMPILER_ATTR_PACK _CbUartConfig {
	CbUartBaudrate baudrate;
	CbUartStopbits stopbits;
	CbUartParity parity;
} CbUartConfig;

typedef struct CB_COMPILER_ATTR_PACK _CbCanConfig {
	CbCanBaudrate baudrate;
} CbCanConfig;

typedef struct CB_COMPILER_ATTR_PACK _CbNetworkConfig {
	char mdns_name[20]; // custom MDNS name
} CbNetworkConfig;

// Final complete config struct

#define CB_CONFIG_VERSION 0
typedef struct CB_COMPILER_ATTR_PACK _cb_config {
	uint32_t config_version;
	SafetyConfig safety;
	CbGpioConfig gpios[CB_NUMBER_OF_GPIOS];
	CbUartConfig uarts[CB_NUMBER_OF_UARTS];
	CbCanConfig can;
	CbNetworkConfig network;
	uint8_t plc_powersaving_mode;
} CbConfig;
