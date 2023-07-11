.. _everest_modules_handwritten_EvseManager:

************************
EvseManager
************************

See also module's :ref:`auto-generated reference <everest_modules_EvseManager>`.

The module ``EvseManager`` is a central module that manages one EVSE 
(i.e. one connector to charge a car).
It may control multiple physical connectors if they are not usable at the same
time and share one connector id,
but one EvseManager always shows as one connector in OCPP for example. So in 
general each connector should have a dedicated EvseManager module loaded.

The EvseManager contains the high level charging logic (Basic charging and 
HLC/SLAC interaction), collects all relevant data for the charging session 
(e.g. energy delivered during this charging session) and provides control over 
the charging port/session. For HLC it uses two helper protocol modules that it 
controls (SLAC and ISO15118).

Protocol modules such as OCPP or other APIs use EvseManagers to control the 
charging session and get all relevant data.

The following charge modes are supported:

* AC charging: Basic Charging according to IEC61851/SAE J1772 and HLC according
  to ISO15118-2
* DC charging: ISO15118-2 and DIN SPEC 70121

Additional features:

* Autocharge support (PnC coming soon)
* Seamlessly integrates into EVerest Energy Management
* The lowest level IEC61851 state machine can be run on a dedicated 
  microcontroller for improved electrical safety
* Support for seperate AC and DC side metering in DC application

Typical connections
===================

TODO: AC and DC module graphs and description

AC Configuration
----------------

DC Configuration
----------------

In DC applications, the EvseManager still has an AC side that behaves similar 
to a normal AC charger. The board_support module therefore still has to report 
AC capabilities which refer to the AC input of the AC/DC power supply. If an AC
side RCD is used it also belongs to the board_support driver. 
An AC side power meter can be connected and it will be used for Energy 
management.

In addition, on the DC side the following hardware modules can be connected:

* A DC powermeter: This will be used for billing purposes if present. 
  If not connected, billing will fall back to the AC side power meter.
* Isolation monitoring: This will be used to monitor isolation during 
  CableCheck, PreCharge and CurrentDemand steps.
* DC power supply: This is the AC/DC converter that actually charges the car.

Published variables
===================

session_events
--------------

EvseManager publishes the session_events variable whenever an event happens. 
It does not publish its internal state but merely events that happen that can 
be used to drive an state machine within another module.

Example: Write a simple module that lights up an LED if the evse is reserved. 
This module requires an EvseManager and subscribes to the session_events 
variable. Internally it has only two states: Reserved (LED on), NotReserved 
(LED off).

The state machine transitions are driven by the two events from EvseManager: 
ReservationStart and ReservationEnd.

All other events are ignored in this module as they are not needed.

powermeter
----------

EvseManager republishes the power meter struct that if it has a powermeter 
connected. This struct should be used for OCPP and display purposes. It comes 
from the power meter that can be used for billing (DC side on DC, AC side on 
AC). If no powermeter is connected EvseManager will never publish this 
variable.


Authentication
==============

The Auth modules validates tokens and assignes tokens to EvseManagers, see Auth
documentation. It will call ``Authorize(id_tag, pnc)`` on EvseManager to 
indicated that the EvseManager may start the charging session. 
Auth module may revoke authorization (``withdraw_authorization`` command) if 
the charging session has not begun yet (typically due to timeout), but not once
charging has started.


Autocharge / PnC
----------------

Autocharge is fully supported, PnC support is coming soon and will use the same
logic. The car itself is a token provider that can provide an auth token to be 
validated by the Auth system (see Auth documentation for more details). 
EvseManager provides a ``token_provider`` interface for that purpose.

If external identification (EIM) is used in HLC (no PnC) then Autocharge is 
enabled by connecting the ``token_provider`` interface to Auth module. When the
car sends its EVCCID in the HLC protocol it is converted to Autocharge format 
and published as Auth token. It is based on the following specification:

https://github.com/openfastchargingalliance/openfastchargingalliance/blob/master/autocharge-final.pdf

To enable PnC the config option ``payment_enable_contract`` must be set to 
true. If the car selects Contract instead of EIM PnC will be used instead of 
Autocharge.

Reservation
-----------

Reservation handling logic is implemented in the Auth module. If the Auth 
module wants to reserve a specific EvseManager (or cancel the reservation) it 
needs to call the reserve/cancel_reservation commands. EvseManager does not 
check reservation id against the token id when it should start charging, this 
must be handled in Auth module. EvseManager only needs to know whether it is 
reserved or not to emit an ReservatonStart/ReservationEnd event to notify other
modules such as OCPP and API or e.g. switch on a specific LED signal on the 
charging port.

Energy Management
=================

EvseManager seamlessly intergrates into the EVerest Energy Management. 
For further details refer to the documentation of the EnergyManager module.

EvseManager has a grid facing Energy interface which the energy tree uses to 
provide energy for the charging sessions. New energy needs to be provided on 
regular intervals (with a timeout). 

If the supplied energy limits time out, EvseManager will stop charging.
This prevents e.g. overload conditions when the network connection drops 
between the energy tree and EvseManager.

EvseManager will send out its wishes at regular intervals: It sends a 
requested energy schedule into the energy tree that is merged from hardware 
capabilities (as reported by board_support module), EvseManager module 
configuration settings 
(max_current, three_phases) and external limts (via ``set_local_max_current`` 
command) e.g. set by OCPP module.

The combined schedule sent to the energy tree is the minimum of all energy 
limits.

After traversing the energy tree the EnergyManager will use this information 
to assign limits (and a schedule) 
for this EvseManager and will call enforce_limits on the energy interface. 
These values will then be used
to configure PWM/DC power supplies to actually charge the car and must not 
be confused with the original wishes that
were sent to the energy tree. 

The EvseManager will never assign energy to itself, it always requests energy 
from the energy manager and only charges
if the energy manager responds with an assignment.

The ``set_local_max_current`` command will be extended to schedules (and not 
just one instantaneous limit) soon to fully
support schedules from OCPP smart charging profile.

Limits in the energy object can be specified in ampere (per phase) and/or watt.
Currently watt limits are unsupported, but it should behave according to that 
logic:

If both are specified also both limits will be applied, whichever is lower. With DC charging, ampere limits apply
to the AC side and watt limits apply to both AC and DC side.
