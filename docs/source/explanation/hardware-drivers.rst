================
Hardware Drivers
================

This chapter describes Hardware Driver modules that are supported
natively by EVerest. The presented components have been prequalified
by Pionix to work with EVerest to ensure a quick path to production.

----------------------------
Isolation Monitoring Devices
----------------------------

Bender ISOMETER isoCHA425
-------------------------

*Hardware Driver Module* :ref:`Bender_isoCHA425HV <everest_modules_Bender_isoCHA425HV>`

You can find more information about the device here:

`<https://www.bender.de/produkte/isolationsueberwachung/isometerr-isocha425hv-mit-agh420-1>`_

Here are the most important specifications:

-  RS485/ModBus connection
-  Up to 1000 V DC with AGH420-1/AGH421-1
-  \< 10s response time
-  Firmware \< 5.00: CableCheck time: Self test ~22s + 10s response time,
   can be used with IEC 61851-23:2014, but cannot be used with IEC
   61851-23:2023 certification
-  Firmware 5.00 improves CableCheck time to allow usage with IEC
   61851-23:2023 (available now from Bender as of Q1/2025). Usage of
   older 4.x firmware is not recommended.
-  AGH421-1 allows full disconnection from the DC wires to allow for
   multiple IMDs on the same wires, one active at a time
-  Measures DC output voltage
-  Measures voltage between DC+/DC- and PE as well
-  Two separate relays to trigger in case of errors

| EVerest supports this device with the "Bender_isoCHA425HV" module.
  The module requires a "SerialCommHub" module to be loaded as well for
  the ModBus communication.
| All settings of the device can be adjusted in the module
  configuration.

The error output relays should be wired directly to the DC output relay
of the charger to enable a low-level emergency shutdown functionality
which works independently of EVerest.

EVerest will read the isolation resistance values and switch off if
they fall below 100 kOhm as well, but safety certification should not
rely on the Linux system.

Dold RN5893 IMD
-------------------------

*Hardware Driver Module* :ref:`Dold RN5893 <everest_modules_DoldRN5893>`

You can find more information about the device here:

`<https://www.dold.com/en/products/relay-modules/monitoring-devices/insulation-monitors/rn-5893>`_

Here are the most important specifications:
- Width:	52,5 mm
- Classification:	For DC charging stations
- Response value:	1 - 500 kΩ
- IMD type:	AC, DC, AC/DC
- Nominal voltage IT system:	AC 0 - 230, AC 0 - 690 (Coupling device), DC 0 - 230, DC 0 - 1000 (Coupling device) V
- Auxiliary voltage:	AC/DC
- Earth fault indicator:	Yes
- Approvals:	UL-Recognized
- Response value type:	Adjustable
- Bus interface:	Modbus RTU
- Enclosure design:	Distribution board
- Type:	RN 5893

| EVerest supports this device with the "DoldRN5893" module.
  The module requires a "SerialCommHub" module to be loaded as well for
  the ModBus communication.
| All settings of the device can be adjusted in the module
  configuration.


---------------
NF/RFID Readers
---------------

Many NXP chips are be supported.
All modules implement the *auth_token_provider* interface and publish a
token to be consumed by matching modules as soon as the NFC chip detects
a compatible RFID card.

NxpNfcFrontendTokenProvider
---------------------------

*Hardware Driver Module* :ref:`NxpNfcFrontendTokenProvider <everest_modules_NxpNfcFrontendTokenProvider>`

The variety of hardware supported by the underlying NxpNfcFrontendWrapper
is limited by the time of writing (only CR663), but can be extended.

This module relies on NXP's proprietary NxpNfcRdLib which users need
to obtain from NXP, due to license reasons (Download is free, but requires
accepting the license terms).

PN532TokenProvider
------------------

*Hardware Driver Module* :ref:`PN532TokenProvider <everest_modules_PN532TokenProvider>`

Supports the PN532 integrated tranceiver.

PN7160TokenProvider
-------------------

*Hardware Driver Module* :ref:`PN7160TokenProvider <everest_modules_PN7160TokenProvider>`

Supports the PNC7160 NFC Controller via the NCI interface.
No Linux kernel module required.

--------------------
AC/DC power supplies
--------------------

Huawei R100040Gx
----------------

*Hardware Driver Module* :ref:`Huawei_R100040Gx <everest_modules_Huawei_R100040Gx>`

The device is supported by EVerest with the "Huawei_R100040Gx" module.

Most important specs:

-  40 kW ACDC with 150 V - 1000 V output
-  low noise fan design
-  ultra compact
-  automatic switching between series and parallel mode
-  stackable
-  CAN bus interface

In the driver configuration, set the addresses of the modules that are
used by this driver. If empty (default), it will use all modules that it
can find on the CAN bus.

If using multiple modules in a stacked configuration, connect the
outputs in parallel and connect all modules to the same CAN bus. Then
specify all module addresses in the module configuration.

Huawei V100R023C10
------------------

*Hardware Driver Module* :ref:`Huawei_V100R023C10 <everest_modules_Huawei_V100R023C10>`

This device is supported in EVerest with the Huawei production firmware.
The setup of this device is complex.
The development kit with unencrypted firmware is not supported by this driver.

Most important specs:

-  Central power unit architecture with satellites, up to 740 kW
-  150 V - 1000 V output, up to 12 satellites
-  Ethernet communication with EVerest
-  Support for multiple connectors on one satellite
-  Full support for production firmware with TLS encryption and GOOSE
   security

Infypower BEG1K075G
-------------------

*Hardware Driver Module* :ref:`InfyPower_BEG1K075G <everest_modules_InfyPower_BEG1K075G>`

Supported by EVerest with the "InfyPower_BEG1K075G" module. Stacking of
multiple modules is not yet supported by the driver.

Most important specs:

-  22 kW bidirectional AC/DC
-  up to 1000 V output voltage

Make sure to update the module to the latest firmware version - older
firmware versions on this converter are known to have bugs that could
result in hardware damages. New firmware is available from InfyPower.

UUGreenPower UR1000X0
---------------------

*Hardware Driver Module* :ref:`UUGreenPower_UR1000X0 <everest_modules_UUGreenPower_UR1000X0>`

Both the 30 kW and 40 kW uni-directional modules from UUGreenPower are
fully supported by EVerest with the "UUGreenPower_UR1000X0" module.

The bidirectional versions are not yet supported, but support is planned
for an upcoming release of EVerest.

Most important specs:

-  30 / 40 kW AC/DC
-  up to 1000 V output voltage
-  automatic series / parallel switching implemented in driver. Can be
   fixed to series or parallel mode in configuration.

If multiple modules are used in a stacked configuration, you must set
the "module\\addresses" configuration parameter to the
addresses of all modules in the stack.

By default, it uses the broadcast address. With multiple modules, this
will result in each module delivering the full current to the EV instead
of sharing the current.

Other AC/DC power supplies
--------------------------

- :ref:`DPM1000 <everest_modules_DPM1000>`
- :ref:`InfyPower <everest_modules_InfyPower>`
- :ref:`Winline <everest_modules_Winline>`

------------
Power meters
------------

DC: LEM DCBM400/600
-------------------

*Hardware Driver Module* :ref:`LemDCBM400600 <everest_modules_LemDCBM400600>`

This power meter is fully supported by EVerest a (LemDCBM400600)
driver.

It supports German Eichrecht regulations and Eichrecht-compliant fault
recovery:

-  After power failure of the complete unit, the transaction is closed
   with the correct signed meter value from the moment the power loss
   happened. It is then also closed in the CSMS if OCPP is used.
-  If a communication loss happens during charging, charging is stopped.
   If the communication is re-established before the EV unplugs, the
   signed meter value is used to close the transaction in the CSMS. If
   it does not re-establish before the EV unplugs, the transaction
   cannot be billed (and no signed meter value will be used to close the
   CSMS transaction).

Version V2 is required to really be Eichrecht-compliant. The driver
auto-detects V1 and V2 hardware versions and supports both.

DC: Acrel DJSF1352
------------------

*Hardware Driver Module* :ref:`Acrel_DJSF1352_RN <everest_modules_Acrel_DJSF1352_RN>`

This is a simple DC power meter with no Eichrecht support. It uses
ModBus/RS485 and requires a SerialCommHub module to work.

DC: AST DC650
-------------

*Hardware Driver Module* :ref:`AST_DC650 <everest_modules_AST_DC650>`

The driver is implemented in the "AST_DC650" module using the SLIP
protocol. Eichrecht support is not complete in the driver in the current
release for all fault cases, but this will come in an upcoming release.

There is a possibility to use it with a REST-based API similar to the
LEM.

DC: DZG GSH01
-------------

*Hardware Driver Module* :ref:`DZG_GSH01 <everest_modules_DZG_GSH01>`

The driver is implemented in the "DZG GSH01" module using the SLIP
protocol.

Note that you need an extra serial port - it cannot be shared with
e.g. other ModBus-based devices.

Eichrecht and OCMF handling are fully implemented.

DC: Isabellenhütte IEM-DCC
--------------------------

*Hardware Driver Module* :ref:`IsabellenhuetteIemDcr <everest_modules_IsabellenhuetteIemDcr>`

There is a driver for the Isabellenhütte IEM-DCC meter in EVerest.

AC: GenericPowermeter
---------------------

*Hardware Driver Module* :ref:`GenericPowermeter <everest_modules_GenericPowermeter>`

The GenericPowermeter driver has support for most ModBus-based AC power
meters. It supports a yaml configuration file for register mapping to
adapt to new power meters.

Example register mappings are included for the following power meters:

-  Eastron SDM72DM, SDM72DM-V2, SDM230, SDM630-V2
-  Klefr 693x - 694x

For all non-Eichrecht power metering, this should be easy to adapt.

AC: Iskra WM3M4 & WM3M4C
------------------------

*Hardware Driver Module* :ref:`RsIskraMeter <everest_modules_RsIskraMeter>`

There is a community driver in the "RsIskraMeter" module. Eichrecht
support is implemented, some fault cases may require additional
handling.

-----------------
Power line modems
-----------------

The following power line modems are supported in the
:ref:`EvseSlac module <everest_modules_EvseSlac>`.
The chip is detected automatically, but each chip may require some
configuration for a real product.

Qualcomm QCA7000/7005/7006
--------------------------

Fully supported including proprietary extensions for link detection and
chip reset.

It is recommended to enable "do_chip_reset" for all Qualcomm chips. This
will reset them after each charging session for improved stability.

If it is booting from the internal flash and correctly configured for
EVSE, it only requires the SPI Linux kernel driver to be loaded (which
is included in the Linux main line).

If it is configured for host boot, additional software outside of
EVerest is required to do firmware loading and configuration for EVSE.

If connected via the Ethernet port on QC7006, no special driver is
needed in Linux (except for the Ethernet interface driver).

Lumissil CG5317
---------------

Fully supported including proprietary extensions for link detection.

Soft reset extensions are not yet in the release, but should not be
necessary for this chip if the latest firmware is being used. Contact
Lumissil to ensure you have the latest firmware.

If it is booting from an externally attached SPI Flash (attached to the
CG5317) and correctly configured for EVSE, it only requires the SPI
Linux kernel driver to be loaded. The kernel driver is open source
licensed but not included in the main line.

If it is configured for host boot, additional software outside of
EVerest is required to do firmware loading and configuration for EVSE.
You can get these tools under NDA from Lumissil.

If connected via the Ethernet port, no special driver is needed in Linux
(except for the Ethernet interface driver).

Vertexcom MSE-102x
------------------

It is auto-detected and supported by the EvseSlac module. Proprietary
extensions for link detection and reset are not yet implemented.
It may be used without those extensions.

If connected via the Ethernet port, no special driver is needed in Linux
(except for the Ethernet interface driver).

----------------------------
BSP for complete controllers
----------------------------

Some controllers are supported natively by EVerest and require no
additional work for Control Pilot, PLC, relay switching etc.

PHYTEC phyVERSO controller
--------------------------

*Hardware Driver Module* :ref:`PhyVersoBSP <everest_modules_PhyVersoBSP>`

phyVERSO is fully supported with all features of the hardware for both
charging ports (AC and DC).

Pionix test hardware
--------------------

All testing hardware from Pionix is fully supported:

- BelayBox
- uMWC

Other BSP modules
-----------------

- :ref:`AdAcEvse22KwzKitBsp <everest_modules_AdAcEvse22KwzKitBsp>`
- :ref:`TIDA010939 <everest_modules_TIDA010939>`
- :ref:`YetiDriverBsp <everest_modules_YetiDriver>`
