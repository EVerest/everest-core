.. _everest_modules_handwritten_AdAcEvse22KwzKitBSP:

************************
AdAcEvse22KwzKitBSP
************************

See also module's :ref:`auto-generated reference <everest_modules_AdAcEvse22KwzKitBSP>`.
The module ``AdAcEvse22KwzKitBSP`` is a board support driver for the Analog Devices 
`AD-ACEVSE22KWZ-KIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-acevse22kwz-kit.html#eb-overview>`_
EVSE reference design.

AdAcEvse22KwzKitBSP Quickstart
==============================

A typical hardware setup would consist of the AD-ACEVSE22KWZ-KIT and a Raspberry Pi 4 running 
EVerest with this module. Communication between AD-ACEVSE22KWZ-KIT and the AdAcEvse22KwzKitBSP 
on the Raspberry Pi 4 is through a 3.3V TTL UART link with a default configuration of 115200 bps 8N1. 
There is also a GPIO used to reset AD-ACEVSE22KWZ-KIT from EVerest so the firmware is in a known state. 

Hardware Connectivity
---------------------

By default, AD-ACEVSE22KWZ-KIT supports the Pionix Yak board and Raspberry Pi 4 as host boards running 
EVerest. 

The simplest way to get AD-ACEVSE22KWZ-KIT up and running with EVerest is with a Pionix Yak board. First,
you must connect AD-ACEVSE22KWZ-KIT's P3 to Yak's J3 using a 10-wire cable. Note that using the 10-wire 
cable has Yak powered off AD-ACEVSE22KWZ-KIT's 12V supply. 

Next, we need to add the AdAcEvse22KwzKitBSP as an active module inside the configuration YAML file 
passed to EVerest:

.. code-block:: yaml

    active_modules:
      # ** Other EVerest modules **
      adacevse22kwz_driver:
        telemetry:
          id: 1
        config_module:
          baud_rate: 115200 # default baud rate
          reset_gpio: 27 # Yak reset GPIO pin
          serial_port: /dev/serial0 # Yak UART0 port
        connections: {}
        module: AdAcEvse22KwzKitBSP
        # ** Other EVerest modules **

Next, we need to link our active AdAcEvse22KwzKitBSP to EvseManager to facilitate high-level charging 
logic. This can be done by adding the following to EvseManager inside the EVerest configuration YAML:

.. code-block:: yaml

    active_modules:
      # ** Other EVerest modules **
      evse_manager:
        config_module:
          # ** Various EvseManager configurations **
        connections:
          bsp:
          - implementation_id: board_support
            module_id: adacevse22kwz_driver
          powermeter_grid_side:
          - implementation_id: powermeter
            module_id: adacevse22kwz_driver
        module: EvseManager
        telemetry:
          id: 1
      # ** Other EVerest modules **

Finally, we can start EVerest. AD-ACEVSE22KWZ-KIT should now be able to communicate with EVerest 
on the Yak board using AdAcEvse22KwzKitBSP.

AD-ACEVSE22KWZ-KIT can also be run off a Raspberry Pi 4 by using various test points on the 
AD-ACEVSE22KWZ-KIT instead of the 10-wire cable. These test points can then be connected to 
Raspberry Pi pins which can operate as a UART link and a standard GPIO:

.. list-table::
    :header-rows: 1

    * - Purpose
      - Test Point
      - RPI Pin
    * - Reset Pin
      - TP_18
      - GPIO27
    * - Host TX
      - TP_28
      - TX_D0
    * - Host RX
      - TP_41
      - RX_D0
    * - Shared ground
      - Any GND pin
      - Any GND pin

You can now follow the steps used for configuring EVerest on the Yak board to start EVerest 
on the Raspberry Pi.

Protocol
========

EVerest can send commands to AD-ACEVSE22KWZ-KIT and AD-ACEVSE22KWZ-KIT publishes 
data and events back to EVerest. The packets are defined with protobuf to serialize the C structs
into a binary representation that is transferred over the serial wire in a 
stream:

https://developers.google.com/protocol-buffers

To be able to split the stream back into packets all data is encoded using COBS
before it is transmitted on the UART:

https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing

COBS
----

COBS is implemented in ``adkit_comms/evSerial.cpp``. Whenever a new packet
was extracted from the stream ``handlePacket()`` is called to decode protobuf
and generate the corresponding signals. 
Other parts of the module subscribe to these signals to handle the incoming 
packets.

For TX ``linkWrite`` encodes the packet with COBS and outputs it to the UART.

Protobuf
--------

The actual packet definitions are located under ``adkit_comms/protobuf``.

``adkit.proto`` contains all messages that can be sent by EVerest and AD-ACEVSE22KWZ-KIT.

Refer to these files for an up-to-date definition as they may change 
frequently.

To generate the C code nanopb is used:

``nanopb_generator -I . -D . *.proto``

The output should also be manually copied to AD-ACEVSE22KWZ-KIT Firmware to ensure the same
definition is used on both sides when making changes.


Modes of Operation
-----------------------------

AD-ACEVSE22KWZ-KIT board operates in the following two modes:

``Hostless mode``: AD-ACEVSE22KWZ-KIT acts as a standalone EVSE 
and will control PWM and relay state without external influence. In this 
mode, PWM will be enabled immediately upon entering state B1 with the 
relay closing in state C2 assuming no errors occur. If an error occurs 
(i.e. RCD trigger, diode short, C1 timeout, etc.), AD-ACEVSE22KWZ-KIT 
will open the relay and disable PWM until state A1 is reentered where 
the errors will be cleared. 

``Host-driven mode``: AD-ACEVSE22KWZ-KIT will allow EVerest to influence PWM
and relay states. In this mode, PWM will not be enabled until an EVerest 
``PwmDutyCycle`` command is received. Similarly, the relay will not open in state 
C2 until an ``AllowPowerOn`` message is received. AD-ACEVSE22KWZ-KIT can 
override relay and PWM state in the event of an error.

By default, the AD-ACEVSE22KWZ-KIT operates in hostless mode until a message
is received from the host. Additionally, all outbound messages from 
AD-ACEVSE22KWZ-KIT are sent irrespective of mode of operation. This enables 
AD-ACEVSE22KWZ-KIT evaluation without using EVerest.

Message types
-------------
AD-ACEVSE22KWZ-KIT supports the following set of messages:

EVerest to AD-ACEVSE22KWZ-KIT:
______________________________

``AllowPowerOn(bool)``: Inform AD-ACEVSE22KWZ-KIT that it is allowed to 
switch on the power relays/contactors to the car on (true) or must switch 
off now (false). The final decision remains with AD-ACEVSE22KWZ-KIT in 
case of power on, it should only power on after all other requirements 
are met (such as RCD current is below limit, car is in CP state C etc). 
On power off, AD-ACEVSE22KWZ-KIT will switch off immediately.

``PwmDutyCycle(uint32)``: Set AD-ACEVSE22KWZ-KIT PWM state and duty 
cycle. PWM can be enabled at specific duty cycle by passing a value of 
1-10000, where each value corresponds to 0.0001% duty cycle (i.e. 50% 
duty cycle = 5000 passed value). AD-ACEVSE22KWZ will ignore any duty cycles 
greater than 5333 as this corresponds to the maximum duty cycle supported.
PWM can be disabled by passing a value greater than 10000. PWM state F can 
be enabled by passing a PWM value of 0.

``KeepAlive(Message)``: EVerest sends this packet to AD-ACEVSE22KWZ-KIT at 1Hz. 
Currently unused by AD-ACEVSE22KWZ-KIT.

``Reset(bool)``: Reset AD-ACEVSE22KWZ-KIT firmware.

AD-ACEVSE22KWZ-KIT to EVerest
-----------------------------

``CpState(enum)``: Notify EVerest of current CP state (A/B/C/D/E/F). Sent upon 
state change. AD-ACEVSE22KWZ-KIT currently doesn't support State D. 

``RelaisState(bool)``: Notify EVerest of current relay state. Sent upon relay 
closing/opening. True corresponds to relay closed and false is sent when relay 
is open.

``PpState(enum)``: Notify EVerest of current PP state (NC/13A/20A/32A/70A/F).
AD-ACEVSE22KWZ-KIT currently doesn't support PP for maximum output current  
so 32A is sent by default.

``PowerMeter(Message)``: Sent roughly every second when relay is closed. 
Contains all data from the ADE9178 power measurement.

``ErrorState(Message)``: Notify EVerest of active errors. Sent when an errors 
are set/cleared. Each error has an associated boolean value where true 
corresponds to active error and false corresponds to error not active. Currently,
only diode faults, RCD triggered, and overcurrent are supported by 
AD-ACEVSE22KWZ-KIT.

``Telemetry(Message)``: Telemetry message with cp pwm high and low voltage 
values. Not currently supported by AD-ACEVSE22KWZ-KIT firmware.

``KeepAliveLo(Message)``: AD-ACEVSE22KWZ-KIT sends this every 3 seconds to keep 
connection online.

``ResetReason(enum)``: Sent once on boot of the AD-ACEVSE22KWZ-KIT firmware.
