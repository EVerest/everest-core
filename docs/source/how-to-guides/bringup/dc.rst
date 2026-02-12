.. _htg_bring_up_dc:

##########
DC BringUp
##########

Make sure to have completed the bring up of the CP signaling as
described in :ref:`htg_basic_bringup`.

For a DC charger, CP is essential. Several other pieces of hardware need
to be brought to life as well, though. This gives you a minimal setup
before testing the complete setup:

- The DC power supply that delivers the charging current to the car
- Isolation monitoring device
- Optional: Proximity pilot

Let's start with the DC power supply as this is the most important one.

DC Power Supply
===============

Make sure the high voltage output of the DC power supply is isolated and
nothing is attached to it that could draw current or get destroyed by
high voltages. Make sure it is safe to set the maximum voltage.

.. warning::

   Follow all relevant safety precautions.
   Only trained personnel may execute this step.
   High voltages may cause severe injury including death.

We will only check the most important features required for a minimal
CCS charger setup. Check IEC 61851-23 for a full set of requirements.

We will manually set voltages up to the maximum voltage and verify the
driver functionality and the performance of the power supply under
no-load conditions.

Create a simple BringUp configuration file that contains only the
BUPowerSupplyDC module and the actual driver module / bridge module to
your external driver. There are a couple of examples that you can
modify: ``config-bringup-huawei.yaml``, ``config-bringup-uugreen.yaml``,
``config-bringup-api-powersupply.yaml``.

Start the BringUp & Qualification session with the following command in
the build folder:

.. code-block:: bash

   /etc/everest/run_tmux_helper.sh /etc/everest/bringup/config-bringup-mypowersupply.yaml /usr

Now make sure to complete the checklist:

Start with the power supply being switched off. Set the maximum output
voltage (e.g. 1000 V) and then switch the power supply to export mode
(rectifier mode, power supply outputs power to the car). Make sure to do
it in exactly this order:

- Verify that the reported capabilities are matching the
  manufacturer's specifications for the power supply.

- Verify that the output voltage is reached within 6 seconds. If it
  stays on a lower voltage, the power supply driver may “forget” the
  voltage setting when being switched on or off. This needs to be
  fixed. The driver should work independently of the order of setting
  output voltage and switching on/off.

- Verify the output voltage and current is measured accurately.
  Compare against an external voltage meter and a current meter (IEC
  61851-1:2023 CC.6.3 requires +- 10 V for voltage and +-1.5% for
  current or 0.5 A - whichever is more).

- Verify that there is no overshoot when changing voltage from lowest
  to highest output voltage. This may cause problems in CableCheck
  phase on some EVs.

- Verify the measured output voltage is correctly reported back to
  EVerest (see measured output voltage in the BringUp module).

- Verify the measured voltage is reported frequently (e.g. every
  second or more often).

- Optional: Measure the power factor. Some power supplies have
  problems with power factor correction under no / light load
  conditions. When charging a real car, no load happens during
  *CableCheck* and *PreCharge* phases.

Now, set the minimum output voltage that the power supply supports
(e.g. 150 V). On power supplies that have high/low mode switching,
choose the minimum voltage that the high mode supports. Otherwise, it
will take quite long to switch from highest to lowest voltage, since the
power supply will need to change its mode configuration.

- Switch the power supply off and verify that the output voltage
  drops to 0 V more or less immediately.

- On power supplies with automatic high/low voltage mode switching:
  Switch manually between the highest and lowest voltage setting to
  trigger mode switches. They should be reasonably fast, e.g. < 5
  seconds. Should switching between the modes be necessary during
  charging, it is ok to have a small pause in *CurrentDemand*.

- Switch on the power supply at e.g. 500 V and exit the bring up
  setup, so EVerest is no longer communicating with the power supply.
  It should switch off after a timeout. (Most power supplies switch off
  within 10-20 seconds after communication loss.)

Isolation monitor device
========================

The isolation monitor needs to comply with IEC 61557-8 or equivalent.
EVerest will use the isolation resistance values as reported by the
driver module and decide whether to stop the charging session or not. It
is a good idea to have a redundant switch-off by e.g. using the IMD’s
built in relay to directly switch the emergency off input of the PSU.

To test the correct functionality within EVerest, use the
BUIsolationMonitor BringUp module and connect it to the driver module.
An example can be found here:

.. code-block:: bash

   config/config-bringup-isolation-monitor-sil.yaml

.. image:: images/bring-up-split-screen.png
   :alt: BUIsolationMonitor Bring-Up

Ideally, the BringUp configuration loads both the isolation monitor and
DC Power supply modules as both are needed to run all tests.

- Switch on the “Power supply” at 500 V. Click on “Start
  measurements”. Verify the isolation monitor sees the 500 V output
  voltage. This is to ensure it is actually connected to the correct
  wires. Verify that new measurements are coming in regularly
  (e.g. every second).

- Verify that the measured resistance is very high (e.g. 1 MOhm or
  so).

- Connect a 100 kOhm (or other value) resistor from the minus wire to
  protective earth. Check how long it takes until the value changes on
  the screen. This should be inline with the specs from the datasheet
  (e.g. <10 seconds for Bender).

- Test the same with the plus wire.

- Stop the measurements and verify no more measurements are coming
  in.

- Start the self-test of the IMD and wait for the result (with the
  power supply still being on). It should take no more than 10 s or so
  to do the self-testing. Many isolation monitors take a long time to
  complete the self-testing. This may cause time-out problems in
  *CableCheck* with some cars.

The time it takes until the IMD detects the isolation fault will need to
be set in the final configuration file.

Over-voltage monitor device
===========================

IEC 61851-23:2023 requires a fast over-voltage protection (OVM)
(6.3.1.106.2). For a minimal proof-of-concept setup in a lab
environment, you may want to skip this as it is not functionally needed
for charging. It will be required for certification though.

The OVM shall switch off if the DC voltage is above the required limit
for 9 ms. For bring-up purposes, we want to test if limit is transported
correctly and if the start and stop commands from EVerest are
implemented correctly in the driver.

An example bring-up configuration using APIs can be found here:

.. code-block:: bash

   config/config-bringup-api-over-voltage-monitor.yaml

Add two things to this bring-up configuration:

1) A power supply driver connected to a BringUp module
2) An evse_board_support driver connected to a BringUp module

Test setup:

- Connect a CP tester to the control pilot and set state C.
- Set “Allow power on” in the evse_board_support BringUp. The relays
  should close now.
- Connect a scope with one channel to the high voltage (use a 1000 V
  probe!) and one channel to coil voltage of the relays. Instead of the
  coil voltage, any other signal that triggers when the emergency
  shutdown starts can be used.

To test the basic functionality, use the check list below. Start each
test with a fresh test setup (relays closed).

.. note::

   There are more requirements in the standard - especially regarding timing.

- Switch on power supply at 500 V. Set the OVM limit to 550 V and
  start the monitoring. Then, set the power supply to 565 V. The relays
  should open immediately. The point in time when the voltage rises
  above 550 V is t_1 - the time where the coil voltage indicates a
  start of emergency shutdown. Verify with the scope that the time
  between t_1 and t_2 is a maximum of 10 ms (9 ms for detection of
  over-voltage and 1 ms to initate the shutdown.)
- Repeat for 825 V (limit) and 840 V (DC output voltage).
- Repeat for 935 V (limit) and 950 V (DC output voltage).
- Repeat for 1100 V (limit) and 1115 V (DC output voltage). This
  measurement is probably not possible as the power supply is limited
  to 1000 V. It is a test case in table 103 of IEC 61851-23:2023.
- Set a limit of 550 V. Switch on the power supply at 500 V. Start
  monitoring. Stop monitoring. Set the DC output voltage to 565 V. The
  relays shall remain closed as the over-voltage monitoring is not
  active.

More IEC 61851-23 test cases for DC
===================================

For a minimum proof-of-concept setup, it should be enough already.

IEC 61851-23 has several more requirements (check the norm for a
complete list) that need to be fulfilled for a real product. Here is the
list of mandatory functions from IEC 61851-23:2023 6.3.1.1 that should
be tested early on as they may influence the final design:

- Verify the timing of the CableCheck phase. EVerest will print some
  timing hints on the console. Several cars will timeout and not charge
  if the Cable check takes more than 30 s. Recommendation is to stay
  below 25 s for the complete phase. This may not be achievable with
  off-the-shelf components. It is however crucial to be compatible with
  all EVs. Use e.g. BYD EVs to test for the 30 s timeout in
  *CableCheck*.

- Continuous continuity checking of the protective conductor
  according to 6.3.1.2: This can be usually fulfilled by opening the
  relays (potentially under load), but you should consider ramping down
  the DC power supply before opening the relays to protect them.

- Verification that the EV is properly connected to the EV supply
  equipment according to 6.3.1.3: This should be compliant
  automatically if the *evse_board_support* and the safety MCU passed
  the BringUp checks.

- Energization of the power supply to the EV according to 6.3.1.4:
  This should be compliant if the BringUp checks are passed.

- De-energization of the power supply to the EV according to 6.3.1.5:
  Similar to “Continuous continuity checking of the protective
  conductor”.

- Maximum allowable current according to 6.3.1.6: This should be
  compliant if the BringUp checks are passed.

- DC supply for EV according to 6.3.1.101: This should be compliant
  if the BringUp checks are passed.

- Measuring current and voltage according to 6.3.1.102: For charging,
  EVerest uses the voltage and current measurements as reported by the
  power supply (not the power meter). This should be compliant if the
  BringUp checks passed for the DC power supply.

- Latching of the vehicle coupler according to 6.3.1.103: This should
  be compliant if the BringUp checks are passed.

- Compatibility check according to 6.3.1.104: This should be
  compliant if the BringUp checks are passed.

- Insulation resistance check before energy transfer according to
  6.3.1.105: This should be compliant if the BringUp checks are passed.

- Protection against over-voltage between DC+ and DC– according to
  6.3.1.106: This is a quite hard requirement (trigger shut-down in 1
  ms after voltage is above limits for 9 ms). It requires both an
  accurate and fast measurement. This needs to be implemented in the
  safety MCU independently of EVerest.

- Verification of vehicle connector latching according to 6.3.1.107:
  Not applicable for CCS.

- Control circuit supply integrity according to 6.3.1.108: Hardware
  requirement.

- Short-circuit check before energy transfer according to 6.3.1.109:
  Test with a 100 Ohm load resistor connected in *CableCheck* as per
  the norm.

- User initiated shutdown according to 6.3.1.110: This can be
  implemented either in the BSP (publish var *request_stop_transaction*
  in interface *evse_board_support*) or use *stop_transaction* command
  in *evse_manager* interface. Both are available via the EVerest API
  modules.

- Overload protection for parallel conductors (conditional function)
  according to 6.3.1.111: Pure hardware requirement.

- Voltage limitation between side B live parts (DC+ and DC–) and
  protective conductor according to 6.3.1.112: Hardware requirement -
  choose especially the IMD wisely.

- Shutdown of EV supply equipment according to 6.3.1.113: This should
  be compliant if the BringUp checks are passed.

First milestone: Connect a real car, zero power charging
========================================================

Now that all individual components are verified, start EVerest with the
full configuration that you created earlier.

Disconnect the plus wire between your prototype and the cable to the EV.
On the first tests, no power should be flowing.

Start EVerest and wait for the message “Ready for charging”. Then plug
in the vehicle. Watch the ISO traffic. It should be successful until the
*PreCharge* phase. Then the EV should stop as it cannot see the voltage.

During the start up of the session, monitor the voltage of the DC power
supply. It should be 500 V in *CableCheck* (or whatever you set in the
config) and then switch to the EV batteries voltage.

Verify that in precharge the output voltage is within +/-20 V of the
EV's battery voltage.

Second milestone: Connect a real car, limited power charging
============================================================

Connect a 5 kOhm resistor in the plus wire between EVSE output and
connector to EV. This will limit the current flow into the EV, but
allows voltages to be seen by the EV. Plug in a vehicle. You should now
get all the way to the *CurrentDemand* phase.

Some EVs will stop after some seconds as no current is flowing.

Verify all communications are correct. Use Wireshark to verify each step
of the ISO communication (see Debug chapter).

Third milestone: Connect a real car, full power charging
========================================================

If everything was correct in the previous step, remove the resistor and
connect the plug normally to the EVSE. Start a charging session.

Power should now be delivered to the EV in *CurrentDemand*.

Verify that the current delivered is what the EV requested and that it
is also reported correctly in the *CurrentDemandRes* messages. Verify it
is shown correctly on the in-vehicle display.

Verify the AC side is not getting overloaded.

.. tip::

   A good habit is to monitor your prototype with a small thermal camera to
   see if any component overheats.

If that works: **Congratulations!** You have successfully charged an EV
with DC for the first time.

Error cases
===========

There are many error cases that should be tested now. Listing all goes
beyond the scope of this manual. A few examples that should be tested:

- Place a 100 kOhm resistor from minus wire to PE. Start a charging
  session. It should also fail in *CableCheck* state.

- Test short circuit under full load.

- Test load dump: Charge at maximum power and open the relays to zero
  load. Watch the voltage overshoot. The power supplies need to survive
  that multiple times. This does happen in the field with some EVs that
  open their contactors during charging when the onboard controller
  firmware resets.

- AC under-/over-voltage input

- Over-temperature shutdown

- On three-phase AC/DC converters, switch off one phase on the input
  at full load.

----

**Authors**: Cornelius Claussen