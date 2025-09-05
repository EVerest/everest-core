// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef CUSTOMIZATION_BOARD_PI4RC663_H
#define CUSTOMIZATION_BOARD_PI4RC663_H

/** Provide RPi4 specific functions (implemented by c-file in src and using existing functions from the NXP DAL)*/
void GPIO_reconfigure_pin(size_t gpio, int output_int);
phStatus_t GPIO_read_pin(size_t gpio, uint8_t *pGpioVal);
phStatus_t GPIO_poll_pin(size_t gpio, int timeOutms);

/** Enable User space SPI. (Required for RC663 frontend chip) */
#define PHDRIVER_LINUX_USER_SPI

/** Choose GPIOs */
#define PHDRIVER_PIN_RESET          536   /** GPIO24 **/
#define PHDRIVER_PIN_IRQ            535   /** GPIO23 */

/** Configure RESET and IRQ pins to pull up/down */
#define PHDRIVER_PIN_RESET_PULL_CFG    PH_DRIVER_PULL_DOWN
#define PHDRIVER_PIN_IRQ_PULL_CFG      PH_DRIVER_PULL_UP

/** Choose trigger type */
#define PIN_IRQ_TRIGGER_TYPE           PH_DRIVER_INTERRUPT_RISINGEDGE

/** Front End Reset logic level settings */
#define PH_DRIVER_SET_HIGH          1
#define PH_DRIVER_SET_LOW           0
#define RESET_POWERDOWN_LEVEL       PH_DRIVER_SET_HIGH
#define RESET_POWERUP_LEVEL         PH_DRIVER_SET_LOW

/** SPI Configuration: "/dev/spidev0.0" @ 5MHz */
#define PHDRIVER_USER_SPI_BUS                     0
#define PHDRIVER_USER_SPI_CS                      0
#define PHDRIVER_USER_SPI_FREQ                    5000000
#define PHDRIVER_USER_SPI_CFG_DIR                 "/dev/spidev"
#define PHDRIVER_USER_SPI_CFG_MODE                SPI_MODE_0
#define PHDRIVER_USER_SPI_CFG_BITS_PER_WORD       8

/** No functionality in this setup (Linux, SPI); just to suppress build errors */
#define PHDRIVER_PIN_SSEL                         0xFFFF
#define PHDRIVER_PIN_NSS_PULL_CFG                 PH_DRIVER_PULL_UP

#endif /* CUSTOMIZATION_BOARD_PI4RC663_H */
