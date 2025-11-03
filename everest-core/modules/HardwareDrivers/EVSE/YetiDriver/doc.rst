.. _everest_modules_handwritten_YetiDriver:

************************
YetiDriver
************************

See also module's :ref:`auto-generated reference <everest_modules_YetiDriver>`.
The module ``YetiDriver`` is a board support driver for Pionix Yeti Power
Board.

Communication between the Yeti microcontroller and this driver module
=====================================================================

The hardware connection between Yeti and Yak (the board running EVerest and
this module) is 3.3V TTL UART plus 2 GPIOs (one to reset the microcontroller
from Linux and one to wakeup Linux from the microcontroller, which is 
currrently unused).

The default configuration is 115200 bps 8N1.

Protocol
========

EVerest can send commands to Yeti and Yeti publishes data and events back
to EVerest. The packets are defined with protobuf to serialize the C structs
into a binary representation that is transferred over the serial wire in a 
stream:

https://developers.google.com/protocol-buffers

To be able to split the stream back into packets all data is encoded using COBS
before it is transmitted on the UART:

https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing

COBS
----

COBS is implemented in ``yeti_comms/evSerial.cpp``. Whenever a new packet
was extracted from the stream ``handlePacket()`` is called to decode protobuf
and generate the corresponding signals. 
Other parts of the module subscribe to these signals to handle the incoming 
packets.

For TX ``linkWrite`` encodes the packet with COBS and outputs it to the UART.

Protobuf
--------

The actual packet definitions are located under ``yeti_comms/protobuf``.

``hi2lo.proto`` contains all messages that can be sent from EVerest to Yeti
while ``lo2hi.proto`` defines all messages that Yeti sends to EVerest.

Refer to these files for an up to date definition as they may change 
frequently.

To generate the C code nanopb is used:

``nanopb_generator -I . -D . *.proto``

The output should also be manually copied to Yeti Firmware to ensure the same
definition is used on both sides when making changes.

EVerest to Yeti
---------------

The most important commands that EVerest sends to Yeti are the following:

``SetControlMode(mode)``: Yeti firmware can operate in different modes:

``Mode NONE = 0``: In this mode Yeti does not allow control over UART. It will
still send telemetry data. Yeti operates as a standalone non-smart AC charger
and EVerest does not need to be running.

``HIGH = 1``: In this mode high level control is possible.
Yeti operates as a standalone AC charger and EVerest does not need to be 
running, but it does allow certain control such as setMaxCurrent from EVerest.
This mode is not documented here as it is not used by EVerest anymore.

``LOW = 2``: In this mode Yeti allows low level control. Yeti does not act
as a standalone charger, it needs to be controlled by EVerest. It does however
still run the very basic state machine to follow the car's states A-F and
switches relais on and off accordingly. This ensures that basic electrical
safety remains within the microcontroller and not within EVerest. 
It generates more human readable events from state A-F transitions.

PWM is directly controlled from EVerest in this mode.

Low control mode:
_________________

The following commands describe the Low level control mode only:

``AllowPowerOn(bool)``: Inform yeti that it is allowed to switch on the power 
relais/contactors to the car on (true) or must switch off now (false). The 
final decision remains with Yeti in case of power on, it should only power on
after all other requirements are met (such as RCD current is below limit,
car is in CP state B etc). On power off Yeti must switch off immediately.

``SetPWM(mode, duty_cycle)``: mode 0: OFF (+12V), 1: ON (PWM with duty_cycle),
 2: F (-12V). Yeti sets the PWM immediately.


Other commands for all modes:
_____________________________

``FirmwareUpdate(bool)``: Send true to reboot Yeti into ROM boot loader. 
After that stm32flash tool can be used to flash any firmware binary to it.
Note that this is a dev kit and for a real product this needs to be implemented
differently.

``KeepAliveHi``: Send this packet to Yeti at 1Hz. If no heartbeat is received
for a longer amount of time Yeti may fall back to control mode NONE to act
as a stand alone emergency backup charger or go into failure mode (can be 
modified in the firmware).

``SetThreePhases``: true: switches to 3ph on next switch on, else single phase.
Only works on hardware configurations with dual relais. Does not switch while
charging session is running, Yeti firmware will delay the change to the next
charging session.

``EnableRCD``: enable or disable the onboard RCD. Some cars generate quite high
residual current spikes and may not charge properly if RCD is enabled.

``Enable``: Enable CP output

``Disable``: Disable CP output (goes to floating/high impedance)

``Reset``: Reset yeti firmware

``Replug``: Initiate special virtual replug sequence without starting a new
charging session.

``SwitchThreePhasesWhileCharging``: Change between 1 and 3 phases while
charging. This is currently not implemented in yeti firmware and will need
special precautions because some cars may be destroyed by switching from one
phase to three phase while charging is running (E.g. Zoe 1)

``ForceUnlock``: Force unlock motor lock now regardless of state.

Yeti to EVerest
---------------

The following messages are relevant for LOW control mode:

``Event``: This is the most important message from Yeti. It will send an event
on CP transitions:

* ``CAR_PLUGGED_IN``: CP State A -> B
* ``CAR_REQUESTED_POWER``: CP State B->C or B->D
* ``POWER_ON``: Relais switched on succesfully (i.e. after mirror contact check)
* ``POWER_OFF``: Relais switched off succesfully
* ``CAR_REQUESTED_STOP_POWER``: CP State C/D -> any other state
* ``CAR_UNPLUGGED``: any other state -> A
* ``ERROR_E``: any other state -> E
* ``ERROR_DF``: Car diode failure detected
* ``ERROR_RELAIS``: Relais error (mirror contact check failed)
* ``ERROR_RCD``:: RCD over current event
* ``ERROR_VENTILATION_NOT_AVAILABLE``: Car requested D but no ventilation available
* ``ERROR_OVER_CURRENT``: Yeti detected quick over current on AC lines
* ``ENTER_BCD``: any other state -> B/C/D. Used to start SLAC
* ``LEAVE_BCD``: B/C/D -> any other state. Stops SLAC.
* ``PERMANENT_FAULT``: Permanent fault that cannot be cleared by unplugging car
* ``EVSE_REPLUG_STARTED``: Replugging sequence started
* ``EVSE_REPLUG_FINISHED``: Replugging sequence completed

``PowerMeter``: Contains all data from the power measurement, sent at roughly
1Hz

``KeepAliveLo``: Yeti sends this at 1Hz to keep up connection.

``ResetDone``: Sent once on boot of yeti firmware.

