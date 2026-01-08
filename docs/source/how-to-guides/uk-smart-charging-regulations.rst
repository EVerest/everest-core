.. _htg_uk_smart_charging_regulations:

#############################
UK Smart Charging Regulations
#############################

The UK imposes additional requirements that may apply to your product if
it is sold in the UK:

https://www.legislation.gov.uk/uksi/2021/1467/contents/made

There are several key requirements in this regulation such as:

-  measuring system and historic logs for 12 months
-  off-peak charging (schedules) pre-set defaults security
-  no default passwords
-  no hard coded credentials
-  encrypted communication (TLS …)
-  checks for updates
-  secure boot
-  check / detect unauthorized software change
-  tamper protection
-  security log (you can use the OCPP security log from EVerest)
-  randomized delays

Charger deployment can also impact the requirements. For example,
randomized delays are disabled when a charger is operating under Demand
Side Response service.

Most of the requirements are not in the EVerest domain and will need to
be implemented by the hardware or non-EVerest software.

Regarding EVerest integration, "Part 2 11 Randomized delays" is
particularly important. EVerest has support for that feature by
enabling the following config option in the
:ref:`EvseManager module <everest_modules_EvseManager>`:

.. code-block:: yaml

   uk_smartcharging_random_delay_enable: true

By setting the config option, EVerest will be compliant with the
regulations by adding a randomized delay of up to ten minutes on any
change of current flow (both increasing and decreasing current).

The delay may be adjusted by the following config option:

.. code-block:: yaml

   uk_smartcharging_random_delay_max_duration: 600

While it is compliant, it is quite annoying to use. The regulation
basically states that a random delay needs to be added if the underlying
reason for the power change is not sufficiently randomized already.

As an example, think about a price signal that is the same for the whole
UK. At 5pm, all EVs will stop charging precisely at the same time
because the prices per kwh increases a lot.

Similarly, if prices drop significantly, all EVs connected will suddenly
start charging.

This is a change in power that definitely shall be randomized to spread
over ten minutes.

Another example would be that the power is driven by the excess energy
from the solar inverter. A small cloud reduces the availability power
and the EV stops charging. As this is a quite local event, it is already
random on a bigger scale. The same applies to users plugging in and out.

EVerest does not know about the nature of the power change, if it is
requested from an external entity (e.g. through OCPP, or a local energy
management system) and hence always applies the delay, even if
unnecessary.

To improve user experience, EVerest supports the
:doc:`uk_random_delay API </reference/interfaces/uk_random_delay>`.
It offers three commands: enable, disable and cancel.

Enable and disable can be used to switch the feature on/off completely
during runtime (e.g. due to user choice).

“Cancel” command can be used to cancel a currently ongoing delay. It has
no effect on the next delay. This should be used to cancel any delay if
the external source of the change knows that it is sufficiently
randomized already.

----

**Authors**: Manuel Ziegler
