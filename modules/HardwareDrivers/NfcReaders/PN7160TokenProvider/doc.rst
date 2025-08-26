.. _everest_modules_handwritten_PN7160TokenProvider:

*******************
PN7160TokenProvider
*******************

This module provides authentication tokens obtained from RFID cards via the NXP PN7160 NFC chip.

It uses a modified *libnfc-nci* as external dependency to interface the chip via I²C or SPI, either from user space or via a kernel module.

Hardware Interface Configuration
================================

Configuration of the hardware interface is possible at runtime.
The module installs two configuration files:

* ``libnfc-nci_config/libnfc-nci.conf``: define NFC options
* ``libnfc-nci_config/libnfc-nxp.conf``: choose the hardware interface (kernel module vs. userspace; I²C vs. SPI, ...)

Module Configuration
====================

The EVerest module can be adjusted in its behaviour as follows:

* ``token_debounce_interval_ms``: Publish tokens in minimum intervall of this timespan in order not to flood subscribers.
* ``disable_nfc_rfid``: Allows to load the module without actually initializing the hardware.

