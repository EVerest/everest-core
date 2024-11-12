.. _everest_modules_handwritten_OCPP:

OCPP1.6 Module
==============

This module implements and integrates OCPP1.6 within EVerest, including all feature profiles defined by the specification. A connection  
to a Charging Station Management System (CSMS) can be established by loading this module as part of the EVerest configuration. This module  
leverages `libocpp <https://github.com/EVerest/libocpp>`_, EVerest's OCPP library.

The EVerest config `config-sil-ocpp.yaml <../../config/config-sil-ocpp.yaml>`_ serves as an example for how to add the OCPP module to  
your EVerest config.

Module configuration
--------------------

Like for every EVerest module, the configuration parameters are defined as part of the module `manifest <manifest.yaml>`_. This module  
is a little special though. The OCPP1.6 protocol defines a lot of standardized configuration keys that are used as part of the functional  
requirements of the specification. These configuration keys mainly influence the control flow of libocpp and are managed by a separate  
JSON configuration file. The module uses the configuration parameter **ChargePointConfigPath** to point to this file.

`This EVerest OCPP tutorial <https://everest.github.io/nightly/tutorials/how_to_ocpp/index.html>`_, the OCPP specification, and  
`libocpp's documentation <https://github.com/EVerest/libocpp>`_ are great resources to learn about the different configuration options.

Integration in EVerest
----------------------

This module leverages `libocpp <https://github.com/EVerest/libocpp>`_, EVerest's OCPP library. Libocpp's approach to implementing the  
OCPP protocol is to do as much work as possible as part of the library. It therefore fulfills a large amount of protocol requirements  
internally. OCPP is a protocol that affects, controls, and monitors many areas of a charging station's operation, though. It is therefore  
required to integrate libocpp with other parts of EVerest. This integration is done by this module and will be explained in this section.

For a detailed description of libocpp and its functionalities, please refer to `its documentation <https://github.com/EVerest/libocpp>`_.

The `manifest <manifest.yaml>`_ of this module defines requirements and implementations of EVerest interfaces to integrate the OCPP  
communication with other parts of EVerest. In order to describe how the responsibilities for functions and operations required by OCPP  
are divided between libocpp and this module, the following sections pick up the requirements of this module and implementations one by one.

Provides: main
^^^^^^^^^^^^^^

**Interface**: `ocpp_1_6_charge_point <../interfaces/ocpp_1_6_charge_point.yaml>`_

This interface is implemented to provide an API to control the websocket connection and to control and retrieve OCPP-specific data like  
security events and configuration keys.

*Note: This interface is deprecated soon and will be removed soon. The functionality is already covered by the generic interface*  
`ocpp <../interfaces/ocpp.yaml>`_ *which is used by this module and OCPP201.*

Provides: auth_validator
^^^^^^^^^^^^^^^^^^^^^^^^

**Interface**: `auth_token_validator <../interfaces/auth_token_validator.yaml>`_

This interface is implemented to forward authorization requests from EVerest to libocpp. Libocpp contains the business logic to either  
validate the authorization request locally using the authorization cache and local authorization list or to forward the request to the  
CSMS using an **Authorize.req**. The implementation also covers the validation of Plug&Charge authorization requests by triggering a  
`DataTransfer.req(Authorize)`.

Provides: auth_provider
^^^^^^^^^^^^^^^^^^^^^^^

**Interface**: `auth_token_provider <../../interfaces/auth_token_provider.yaml>`_

This interface is implemented to publish authorization requests from the CSMS within EVerest. An authorization request from the CSMS is  
turned out by a **RemoteStartTransaction.req**.

Provides: data_transfer
^^^^^^^^^^^^^^^^^^^^^^^

**Interface**: `ocpp_data_transfer <../../interfaces/ocpp_data_transfer.yaml>`_

This interface is implemented to provide a command to initiate a **DataTransfer.req** from the charging station to the CSMS.

Provides: ocpp_generic
^^^^^^^^^^^^^^^^^^^^^^

**Interface**: `ocpp <../../interfaces/ocpp.yaml>`_

This interface is implemented to provide an API to control an OCPP service and to set and get OCPP-specific data.

Provides: session_cost
^^^^^^^^^^^^^^^^^^^^^^

**Interface**: `session_cost <../../interfaces/session_cost.yaml>`_

This interface is implemented to publish session costs received by the CSMS as part of the California Pricing whitepaper extension.

Requires: evse_manager
^^^^^^^^^^^^^^^^^^^^^^

**Interface**: `evse_manager <../../interfaces/evse_manager.yaml>`_

Typically the `EvseManager <../EvseManager/>`_ module is used to fulfill this requirement.

This module requires (1-128) implementations of this interface in order to integrate with the charge control logic of EVerest. One  
connection represents one EVSE. In order to manage multiple EVSEs via one OCPP connection, multiple connections need to be configured  
in the EVerest config file.

This module makes use of the following commands of this interface:

* **get_evse** to get the EVSE id of the module implementing the **evse_manager** interface at startup
* **pause_charging** to pause charging in case a **StopTransaction.conf** indicates charging shall be paused
* **resume_charging** to resume charging
* **stop_transaction** to stop a transaction in case the CSMS stops a transaction by e.g. a **RemoteStopTransaction.req**
* **force_unlock** to force the unlock of a connector in case the CSMS sends a **UnlockConnector.req**
* **enable_disable** to set the EVSE to operative or inoperative, e.g., in case the CSMS sends a **ChangeAvailability.req**. This command can  
  be called from different sources. It therefore contains an argument **priority** in order to override the status if required. OCPP uses  
  a priority of 5000, which is mid-range.
* **set_external_limits** to apply a power or ampere limits at the EVSE received by the CSMS using the SmartCharging feature profile.  
  Libocpp contains the business logic to calculate the composite schedule for received charging profiles. This module gets notified in  
  case charging profiles are added, changed, or cleared. When notified, this module requests the composite schedule from libocpp and  
  publishes the result via the `Provides: ocpp_generic <#provides-ocpp_generic>`_ interface. The duration of the composite schedule can  
  be configured by the configuration parameter **PublishChargingScheduleDurationS** of this module. The configuration parameter  
  **PublishChargingScheduleIntervalS** defines the interval to use to periodically retrieve and publish the composite schedules.
* **set_get_certificate_response** to report that the charging station received a **DataTransfer.conf(Get15118EVCertificateResponse)** from  
  the CSMS (EV Contract installation for Plug&Charge)
* **external_ready_to_start_charging**: To signal that the module has started to establish an OCPP connection to the CSMS

The interface is used to receive the following variables:

* **powermeter** to push powermeter values of an EVSE. Libocpp initiates **MeterValues.req** internally and is responsible to comply with  
  the configured intervals and measurands for clock-aligned and sampled meter values.   
* **ev_info** to obtain the state of charge (SoC) of an EV. If present, this is reported as part of a **MeterValues.req**
* **limits** to obtain the current offered to the EV. If present, this is reported as part of a **MeterValues.req**
* **session_event** to trigger **StatusNotification.req**, **StartTransaction.req**, and **StopTransaction.req** based on the reported event.  
  This signal drives the state machine and the transaction handling of libocpp.
* **iso15118_certificate_request** to trigger a **DataTransfer.req(Get15118EVCertificateRequest)** as part of the Plug&Charge process
* **waiting_for_external_ready** to obtain the information that a module implementing this interface is waiting for an external ready signal
* **ready** to obtain a ready signal from a module implementing this interface

Requires: connector_zero_sink
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Interface**: `external_energy_limits <../../interfaces/external_energy_limits.yaml>`_

Typically the `EnergyNode <../EnergyNode/>`_ module is used to fulfill this requirement.

This module optionally requires the connection to a module implementing the **external_energy_limits** interface. This connection is used  
to apply power or ampere limits at EVSE id zero received by the CSMS using the SmartCharging feature profile.

This module makes use of the following commands of this interface:

* **set_external_limits** to apply a power or ampere limits for EVSE id zero (the whole charging station).

Requires: reservation
^^^^^^^^^^^^^^^^^^^^^

**Interface**: `reservation <../../interfaces/reservation.yaml>`_

Typically the `Auth <../Auth/>`_ module is used to fulfill this requirement.

This module requires a connection to a module implementing the `reservation` interface. This connection is used to apply reservation  
requests from the CSMS.

This module makes use of the following commands of this interface:

* **reserve_now** which is called when the CSMS sends a **ReserveNow.req**
* **cancel_reservation** which is called when the CSMS sends a **CancelReservation.req**

Requires: auth
^^^^^^^^^^^^^^

**Interface**: `auth <../../interfaces/auth.yaml>`_

Typically the `Auth <../Auth/>`_ module is used to fulfill this requirement.

This module requires a connection to a module implementing the **auth** interface. This connection is used to set the standardized  
**ConnectionTimeout** configuration key if configured and/or changed by the CSMS.

This module makes use of the following commands of this interface:

* **set_connection_timeout** which is e.g., called in case the CSMS uses a **ChangeConfiguration.req(ConnectionTimeout)**

Requires: system
^^^^^^^^^^^^^^^^

**Interface**: `system <../../interfaces/system.yaml>`_

The `System <../System/>`_ module can be used to fulfill this requirement. Note that this module is not meant to be used in  
production systems without any modification!

This module requires a connection to a module implementing the **system** interface. This connection is used to execute and control  
system-wide operations that can be triggered by the CSMS, like log uploads, firmware updates, and resets.

This module makes use of the following commands of this interface:

* **update_firmware** to forward **UpdateFirmware.req** or **SignedUpdateFirmware.req** messages from the CSMS
* **allow_firmware_installation** to notify the module that the installation of the firmware is now allowed. A prerequisite for this  
  is that all EVSEs are set to inoperative. This module and libocpp take care of setting the EVSEs to inoperative before calling  
  this command.
* **upload_logs** to forward **GetDiagnostics.req** or **GetLog.req** messages from the CSMS
* **is_reset_allowed** to check if a **Reset.req** message from the CSMS shall be accepted or rejected
* **reset** to perform a reset in case of a **Reset.req** message from the CSMS
* **set_system_time** to set the system time communicated by a **BootNotification.conf** or **Heartbeat.conf** messages from the CSMS
* **get_boot_reason** to obtain the boot reason to use it as part of the **BootNotification.req** at startup

The interface is used to receive the following variables:

* **log_status** to obtain the log update status. This triggers a **LogStatusNotification.req** or **DiagnosticsStatusNotification.req**  
  message to inform the CSMS about the current status. This signal is expected as a result of an **upload_logs** command.
* **firmware_update_status** to obtain the firmware update status. This triggers a **FirmwareStatusNotification.req** or  
  **SignedFirmwareStatusNotification.req** message to inform the CSMS about the current status. This signal is expected as a result  
  of an **update_firmware** command.

Requires: security
^^^^^^^^^^^^^^^^^^

**Interface**: `evse_security <../../interfaces/evse_security.yaml>`_

This module requires a connection to a module implementing the `evse_security` interface. This connection is used to execute  
security-related operations and to manage certificates and private keys.

Typically the `EvseSecurity <../EvseSecurity/>`_ module is used to fulfill this requirement.

This module makes use of the following commands of this interface:

* **install_ca_certificate** to handle **InstallCertificate.req** and **DataTransfer.req(InstallCertificate) messages from the CSMS
* **delete_certificate** to handle **DeleteCertificate.req** and **DataTransfer.req(DeleteCertificate)** messages from the CSMS
* **update_leaf_certificate** to handle **CertificateSigned.req** and **DataTransfer.req(CertificateSigned) messages from the CSMS
* **verify_certificate** to verify certificates from the CSMS that are sent as part of **SignedUpdateFirmware.req**
* **get_installed_certificates** to handle **GetInstalledCertificateIds.req** and **DataTransfer.req(GetInstalledCertificateIds)**
  messages from the CSMS
* **get_v2g_ocsp_request_data** to update the OCSP cache of V2G sub-CA certificates using a **DataTransfer.req(GetCertificateStatus)**.  
  Triggering this message is handled by libocpp internally
* **get_mo_ocsp_request_data** to include the **iso15118CertificateHashData** as part of a **DataTransfer.req(Authorize)** for Plug&Charge  
  if required
* **update_ocsp_cache** to update the OCSP cache which is part of a **DataTransfer.conf(GetCertificateStatus)** message from the CSMS
* **is_ca_certificate_installed** to verify if a certain CA certificate is installed
* **generate_certificate_signing_request** to generate a CSR that can be used as part of a **SignCertificate.req** and  
  `DataTransfer.req(SignCertificate)` message to the CSMS.
* **get_leaf_certificate_info** to get the certificate and private key path of the CSMS client certificate used for security profile 3.
* **get_verify_file** to get the path to a CA bundle that can be used for verifying, e.g., the CSMS TLS server certificate
* **get_leaf_expiry_days_count** to determine when a leaf certificate expires. This information is used by libocpp in order to renew  
  leaf certificates in case they expire soon

Note that a lot of conversion between the libocpp types and the generated EVerest types are required for the given commands. Since the  
conversion functionality is used by this OCPP module and the OCPP201 module, it is implemented as a
`separate library <../../lib/staging/ocpp/>`_ .

Requires: data_transfer
^^^^^^^^^^^^^^^^^^^^^^^

**Interface**: `ocpp_data_transfer <../../interfaces/ocpp_data_transfer.yaml>`_

This module optionally requires a connection to a module implementing the **ocpp_data_transfer** interface. This connection is used to  
handle **DataTransfer.req** messages from the CSMS. A module implementing this interface can contain custom logic to handle the requests  
from the CSMS.

This module makes use of the following commands of this interface:

* **data_transfer** to forward **DataTransfer.req** messages from the CSMS

Requires: display_message
^^^^^^^^^^^^^^^^^^^^^^^^^

**Interface**: `display_message <../../interfaces/display_message.yaml>`_

This module optionally requires a connection to a module implementing the **display_message** interface. This connection is used to allow  
the CSMS to display pricing or other information on the display of a charging station. In order to fulfill the requirements of the  
California Pricing whitepaper, it is required to connect a module implementing this interface.

This module makes use of the following commands of this interface:

* **set_display_message** to set a message on the charging station's display. This is executed when the CSMS sends a  
  **DataTransfer.req(SetUserPrice)** message to the charging station.

Global Errors and Error Reporting
---------------------------------

The **enable_global_errors** flag for this module is enabled in its manifest. This module is therefore able to retrieve and process all reported errors  
from other modules loaded in the same EVerest configuration.

In OCPP1.6 errors can be reported using the **StatusNotification.req** message. If this module gets notified about a raised error,  
it initiates a **StatusNotification.req** that contains information about the error that has been raised.

The field **status** of the **StatusNotification.req** will be set to faulted only in case the error is of the special type  
**evse_manager/Inoperative**. The field **connectorId** is set based on the mapping (for EVSE id and connector id) of the origin of the error.  
If no mapping is provided, the error will be reported on connectorId 0. Note that the mapping can be configured per module inside the  
EVerest config file.

For all other errors, raised in EVerest, the following mapping to an
OCPP **StatusNotification.req** will be used:

* **StatusNotification.req** property ``errorCode`` will always be
  ``OtherError``
* **StatusNotification.req** property ``status`` will reflect the present status of the
  charge point
* **StatusNotification.req** property ``info`` -> origin of EVerest error
* **StatusNotification.req** property ``vendorErrorCode`` -> EVerest error type and
  subtype (the error type is simplified, meaning, that its leading part,
  the interface name, is stripped)
* **StatusNotification.req** property ``vendorId`` -> EVerest error message

The reason for using the **StatusNotification.req** property property
``vendorId`` for the error message is that it can carry the largest
string (255 characters), whereas the other fields (``info`` and
``vendorErrorCode``) only allow up to 50 characters.

If for example the module with id `yeti_driver` within its
implementation with id `board_support` creates the following error:

.. code-block:: cpp

  error_factory->create_error("evse_board_support/EnergyManagement",
                              "OutOfEnergy", "someone cut the wires")

the corresponding fields in the **StatusNotification.req** message will
look like this:

.. code-block:: JSON

  {
    "info": "yeti_driver->board_support",
    "vendorErrorCode": "EnergyManagement/OutOfEnergy",
    "vendorId": "someone cut the wires"
  }

The **StatusNotification.req** message has some limitations with respect
to reporting errors:

* Single errors cannot simply be cleared. If multiple errors are raised,
  it is not possible to clear individual errors.
* ``vendorId``, ``info`` and ``vendorErrorCode`` are limited in length
  (see above).

This module attempts to follow the Minimum Required Error Codes (MRECS): https://inl.gov/chargex/mrec/. This proposes a unified  
methodology to define and classify a minimum required set of error codes and how to report them via OCPP1.6.

This module currently deviates from the MREC specification in the following points:

* Simultaneous errors: MREC requires reporting simultaneous errors by reporting them in a single **StatusNotification.req** by separating 
  the information of the fields **vendorId** and **info** by a semicolon. This module sends one **StatusNotification.req** per individual error 
  because of the limited maximum characters of the **info** field.
* MREC requires always using the value **Faulted** for the **status** field when reporting an error. The OCPP1.6 specification defines the 
  **Faulted** value as follows: "When a Charge Point or connector has reported an error and is not available for energy delivery.  
  (Inoperative)." This module, therefore, only reports **Faulted** when the Charge Point is not available for energy delivery.

Energy Management and Smart Charging Integration
------------------------------------------------

OCPP1.6 defines the SmartCharging feature profile to allow the CSMS to control or influence the power consumption of the charging station. 
This module integrates the composite schedule(s) within EVerest's energy management. For further information about smart charging and the
composite schedule calculation please refer to the OCPP1.6 specification.

The integration of the composite schedules is implemented through the optional requirement(s) `evse_energy_sink` (interface: `external_energy_limits`) 
of this module. Depending on the number of EVSEs configured, each composite limit is communicated via a seperate sink, including the composite schedule
for EVSE with id 0 (representing the whole charging station). The easiest way to explain this is with an example. If your charging station
has two EVSEs you need to connect three modules that implement the `external_energy_limits` interface: One representing evse id 0 and 
two representing your actual EVSEs.

📌 **Note:** You have to configure an evse mapping for each module connected via the evse_energy_sink connection. This allows the module to identify
which requirement to use when communicating the limits for the EVSEs. For more information about the module mapping please see 
`3-tier module mappings <https://everest.github.io/nightly/general/05_existing_modules.html#tier-module-mappings>`_.

This module defines a callback that gets executed every time charging profiles are changed, added or removed by the CSMS. The callback retrieves
the composite schedules for all EVSEs (including evse id 0) and calls the `set_external_limits` command of the respective requirement that implements
the `external_energy_limits` interface. In addition, the config parameter `PublishChargingScheduleIntervalS` defines a periodic interval to retrieve
the composite schedule also in case no charging profiles have been changed. The configuration parameter `PublishChargingScheduleDurationS` defines 
the duration in seconds of the requested composite schedules starting now. The value configured for `PublishChargingScheduleDurationS` shall be greater
than the value configured for `PublishChargingScheduleIntervalS` because otherwise time periods could be missed by the application.


Certificate Management
----------------------

Two leaf certificates are managed by the OCPP communication enabled by this module:

* CSMS Leaf certificate (used for mTLS for SecurityProfile3)
* SECC Leaf certificate (Server certificate for ISO15118)

60 seconds after the first **BootNotification.req** message has been accepted by the CSMS, the charging station will check if the existing 
certificates are not present or have been expired. If this is the case, the charging station initiates the process of requesting a new
certificate by sending a certificate signing request to CSMS.

For the CSMS Leaf certificate, this process is only triggered if SecurityProfile 3 is used.

For the SECC Leaf certificate, this process is only triggered if Plug&Charge is enabled by setting the **ISO15118PnCEnabled** to **true**.

If a certificate has expired is then periodically checked every 12 hours.

In addition to that, the charging station periodically updates the OCSP responses of the sub-CA certificates of the V2G certificate chain.
The OCSP response is cached and can be used as part of the ISO15118 TLS handshake with EVs. The OCSP update is by default performed 
every seven days (or can be configured using the **OCSPRequestInterval** configuration key). 
The timestamp of the last update is stored persistently, so that this process is not necessarily performed at every start up.

