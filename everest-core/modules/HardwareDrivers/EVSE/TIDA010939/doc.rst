.. _everest_modules_handwritten_TIDA010939:

************************
TIDA010939
************************

See also module's :ref:`auto-generated reference <everest_modules_TIDA010939>`.
The module ``TIDA010939`` is a board support driver for Texas Instruments
TIDA-010939 reference design. It is based on the Yeti driver with similar structure
and functionality.

Communication between the TIDA010939 microcontroller and this driver module
============================================================================

The hardware connection between TIDA010939 and the host system (the board running EVerest and
this module) is 3.3V TTL UART plus 2 GPIOs (one to reset the microcontroller
from Linux and one to boot into the bootloader).

The default configuration is 115200 bps 8N1.

Protocol
========

EVerest can send commands to TIDA010939 and TIDA010939 publishes data and events back
to EVerest. The packets are defined with protobuf to serialize the C structs
into a binary representation that is transferred over the serial wire in a 
stream:

https://developers.google.com/protocol-buffers

To be able to split the stream back into packets all data is encoded using COBS
before it is transmitted on the UART:

https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing

COBS
----

COBS is implemented in ``tida_010939_comms/evSerial.cpp``. Whenever a new packet
was extracted from the stream ``handlePacket()`` is called to decode protobuf
and generate the corresponding signals. 
Other parts of the module subscribe to these signals to handle the incoming 
packets.

For TX ``linkWrite`` encodes the packet with COBS and outputs it to the UART.

Protobuf
--------

The actual packet definitions are located under ``tida_010939_comms/protobuf``.

``tida_010939.proto`` contains all messages that can be sent from EVerest to TIDA010939 and
all messages that TIDA010939 sends to EVerest.

Refer to these files for an up to date definition as they may change 
frequently.

To generate the C code nanopb is used:

``nanopb_generator -I . -D . *.proto``

The output should also be manually copied to TIDA010939 Firmware to ensure the same
definition is used on both sides when making changes.

References
============
`Official website https://www.ti.com/tool/TIDA-010939 <https://www.ti.com/tool/TIDA-010939>`_
