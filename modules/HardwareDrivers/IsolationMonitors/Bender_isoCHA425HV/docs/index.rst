.. _everest_modules_handwritten_Bender_isoCHA425HV:

.. ******************
.. Bender_isoCHA425HV
.. ******************

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
* disable the system connection test "nEt" until the first self test started by EVerest  [register 3024]
* disable the self test at device start  [register 3025]
* disable device (only if the configuration option ``threshold_resistance_kohm`` is set to its maximum of 600 [k Ohms]) [register 3026]

Startup and operation
=====================

To start the device and begin insulation (and voltage) measurements, send a "start" command via the device's MQTT interface. The device will then begin sending its measurement data once a second.

After the desired measurement has been retrieved, disable the device via an MQTT "stop" command on its interface.

Devices with firmware version newer than 5.00 support a faster self test mode to reduce CableCheck time. This will automatically be used if supported by firmware.

Self test and system connection test
------------------------------------

The system connection test "nEt" (register 3024) detects an interrupted measuring lead L1/+ or L2/- (device error
E.02). It needs a monitored system with a low internal resistance and, in the "on U" setting, a system voltage above
DC 100 V during the test, so it cannot pass at idle. As the device stores this parameter permanently and would also
use it for its own self test at power-on, the driver handles both registers as follows, writing register 3024 only
when the selection changes:

.. list-table::
   :header-rows: 1

   * - ``selftest_enable_gridconnection``
     - Driver start
     - Self test at driver start, recovery device test
     - Self test, test voltage <= 100 V
     - Self test, test voltage > 100 V
     - Between tests
   * - false (default)
     - off
     - off
     - off
     - off
     - off
   * - true
     - off
     - off
     - on
     - on U
     - last selection

The device's self test at power-on (register 3025) is always disabled. With ``selftest_enable_at_start`` the driver
runs one self test when it starts instead; a device error found by it is reported as DeviceFault. A self test
requested before this test has ended, from the start of the device configuration on, is started after it. A device
test started at the device itself (test button) between tests uses the last selection; if it fails the system
connection test, the recovery described below clears the device error.

A device error is raised as DeviceFault in whichever measurement channel the device reports it (E.01 in R_F, E.02 in
U_N) and also while a self test is running, so an interrupted measuring lead fails the cable check as soon as the
self test detects it. A missing or short Modbus reply raises DeviceFault as well but is not a device error: the
driver keeps the alarm and test state of the last complete read, so a lost reply neither ends a running self test
nor starts the recovery.

The device keeps a device error and the alarm relays until a device test passes; clearing the error memory (register
8006) does not help. The driver therefore starts a device test without system connection test at its next read at
least 5 s after it sees a device error with no self test running (it reads every 10 s while DeviceFault is raised),
whether a failed system connection test, the self test at start or the device itself raised it. While the device
still answers "busy" the start is retried at the following reads up to six times, afterwards every 5 min; a device
error still present after the device test is retried every 5 min. DeviceFault stays raised until the device test
passes. A self test requested while a device test is running or a device error is active fails immediately, so a
device test without system connection test never passes as the cable check self test.

Using the internal alarm functions and relays
=============================================

The external alarm relays should be used to ensure shut down of the DC HV voltage independently from the Linux host. Make sure to configure the thresholds correctly.
