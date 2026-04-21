.. _everest_modules_handwritten_EvseV2G:

.. *******************************************
.. EvseV2G
.. *******************************************

This module includes a DIN70121 and ISO15118-2 implementation provided by chargebyte GmbH

Feature List
============

This document contains feature lists for DIN70121 and ISO15118-2 features, which EvseV2G supports.
These lists serve as a quick overview of which features are supported.

DIN70121
--------

===============  ==================
Feature          Supported
===============  ==================
DC               ✔️
ExternalPayment  ✔️
===============  ==================

ISO15118-2
----------

=======================  ==================
Feature                  Supported
=======================  ==================
AC                       ✔️
DC                       ✔️
TCP & TLS 1.2            ✔️
ExternalPayment          ✔️
Plug&Charge              ✔️
CertificateInstallation  ✔️
CertificateUpdate        
Pause/Resume             ✔️
Schedule Renegotation    ✔️
Smart Charging           ✔️
Internet Service
=======================  ==================

Smart Charging (HLC schedule bundle handoff)
--------------------------------------------

EvseV2G receives the OCPP CSMS charging schedule bundle via the
``iso15118_extensions`` interface and translates up to three
``ChargingSchedule`` entries into ``SAScheduleTuples`` on the iso2 wire
(``PMaxSchedule``). ``ChargeParameterDiscoveryRes`` is gated at
``EVSEProcessing=Ongoing`` until the bundle arrives or the self-heal
deadline elapses.

**Configuration**

.. list-table::
   :header-rows: 1

   * - Key
     - Default
     - Description
   * - ``cpd_timeout_ms``
     - ``60000``
     - Deadline in milliseconds for the OCPP schedule bundle to arrive
       after the EV enters ChargeParameterDiscovery. If the deadline
       elapses, a fallback ``SAScheduleList`` is synthesized so the EV
       can proceed. Values ``<= 0`` warn and fall back to the default.
       Bundles arriving after CPD has completed are dropped on the wire.

Schedule Renegotiation (K16)
----------------------------

When the CSMS updates a charging profile that materially changes the
composite, OCPP201 invokes the ``trigger_schedule_renegotiation`` command
on this interface. EvseV2G latches ``EVSENotification=ReNegotiation`` so
the next ``ChargingStatusRes`` / ``CurrentDemandRes`` drives the EV back
into ``ChargeParameterDiscovery`` where the refreshed bundle is served.
EV-initiated renegotiation (K15.FR.09 boundary violation on the
EV-returned schedule) follows the same latch path.
