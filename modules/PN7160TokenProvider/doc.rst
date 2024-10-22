.. _everest_modules_handwritten_PN7160TokenProvider:

*******************
PN7160TokenProvider
*******************

This module provides authentication tokens obtained from RFID cards via the NXP PN7160 NFC chip.

It uses *libnfc-nci* (see ``everest-core/lib/staging/linux_libnfc-nci``) to interface the chip via I²C or SPI, either from user space or via a kernel module.

Hardware Details Configuration
==============================

Configuration of the hardware interface is required in the library source code:

* ``everest-core/lib/staging/linux_libnfc-nci/conf/libnfc-nxp.conf`` allows to choose the hardware interface (``NXP_TRANSPORT``).
* ``src/nfcandroid_nfc_hidlimpl/halimpl/tml/transport/NfccAltTransport.h`` allows to define the GPIO pins, I²C address and I²C/SPI device.

Module Configuration
====================

The EVerest module can be adjusted in its behaviour as follows:

* ``token_debounce_interval_ms``: Publish tokens in minimum intervall of this timespan in order not to flood subscribers.
* ``disable_nfc_rfid``: Allows to load the module without actually initializing the hardware.

