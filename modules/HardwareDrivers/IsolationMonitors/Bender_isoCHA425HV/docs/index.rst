.. _everest_modules_handwritten_Bender_isoCHA425HV:

*******************************************
Bender_isoCHA425HV
*******************************************

:ref:`Link <everest_modules_Bender_isoCHA425HV>` to the module's reference.

IMD driver for Bender isoCHA IMD devices

Default settings
================

By default the Bender IsoCHA type insulation monitor devices come pre-configured with the following settings:

* 19200 baud / even parity / 1 stop-bit
* bus address: 3

If you want to change those settings, you need to use the manufacturer's configuration software or change the settings on the device's LCD display and buttons.

Bus termination
----------------

The Bender IsoCHA devices come with an internal 120 Ohms switchable bus termination resistor on the modbus connection terminals. To use it, put the switch into the upper (on) position.

Settings overwritten by the driver
----------------------------------

During initialization, this driver overwrites the following settings:

* set "pre-alarm" resistor R1 (default = 600k)  [register 3005 and following]
* set "alarm" resistor R2 to slightly lower than R1 (= -10k)  [register 3007]
* disable low-voltage alarm  [register 3008 and following]
* disable overvoltage alarm  [register 3010 and following]
* set mode to "dc"  [register 3023 and following]
* disable automatic self-tests  [register 3021 and following]
* disable line voltage test  [register 3024]
* disable device (only if the configuration option ``threshold_resistance_kohm`` is set to its maximum of 600 [k Ohms]) [register 3026]

Startup and operation
=====================

To start the device and begin insulation (and voltage) measurements, send a "start" command via the device's MQTT interface. The device will then begin sending its measurement data once a second.

After the desired measurement has been retrieved, disable the device via an MQTT "stop" command on its interface.

Devices with firmware version newer than 5.00 support a faster self test mode to reduce CableCheck time. This will automatically be used if supported by firmware.

Using the internal alarm functions and relays
=============================================

The external alarm relays should be used to ensure shut down of the DC HV voltage independently from the Linux host. Make sure to configure the thresholds correctly.
