.. _everest_modules_handwritten_ImdBenderIsoCha:

*******************************************
ImdBenderIsoCha
*******************************************

:ref:`Link <everest_modules_ImdBenderIsoCha>` to the module's reference.

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
* disable self-test on power-up  [register 3025]
* disable device (only if the configuration option ``threshold_resistance_kohm`` is set to its maximum of 600 [k Ohms]) [register 3026]

Startup and operation
=====================

After the driver has been initialized, the device will be in "stop" mode and completely passive (to ensure that no other functionality is being disturbed).

To start the device and begin insulation (and voltage) measurements, send a "start" command via the device's MQTT interface. The device will then begin sending its measurement data once a second.

After the desired measurement has been retrieved, disable the device via an MQTT "stop" command on its interface. This will prevent further measurements until the next "start" command has been received.

Using the internal alarm functions and relays
=============================================

The default application of the Bender IsoCHA device in EVerest is as a sensor for insulation resistance. Thus, the normal usage does not include the device's internal relays, nor its alarm contacts. To that end, the default trigger resistance is set to maximum (R1 = 600k Ohms; R2 = 590k Ohms). To change these values, you can set the configuration option ``threshold_resistance_kohm``, which can set R1 between 15k Ohms and 600k Ohms. R2 (pre-alarm) will always be set 10k Ohms lower than R1.

**Please note, that the alarm states are not being transmitted by EVerest, as it has its own alarm mechanisms based on the resistance values of ``resistance_F_Ohms`` of the ``isolation_monitor`` interface.** 

*EVerest will manage its settings for limits, alarms and alarm-reactions separately in its EVSE manager!*

**ATTENTION: If the device is to be used for autonomous switching of its internal relays, the configuration option** ``threshold_resistance_kohm`` **MUST be set to a different value than its default of 600 [k Ohms], else the device will be disabled if not explicitly started via its MQTT interface!**

*Note: Even if the configuration option* ``threshold_resistance_kohm`` *has been set to a different value than its default, the device will not perform automatic or power-on self-tests as those might interfere with other parts of EVerest!*