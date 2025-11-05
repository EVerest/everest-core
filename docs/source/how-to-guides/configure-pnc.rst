.. _howto-configure-pnc:

Configure Plug&Charge in EVerest
================================

This is a goal-oriented how-to-guide on how to configure Plug&Charge in EVerest.

To learn how Plug&Charge is implemented in EVerest, please refer to the
:doc:`Explanation of the Plug&Charge process </explanation/pnc-process>`.

The following two configuration files are relevant and require a correct setup and activation for Plug&Charge:

* EVerest configuration file (yaml)
* OCPP configuration file(s) (json) for OCPP 1.6 or OCPP 2.x

Let's start with the EVerest configuration file. If you haven't read
:ref:`Explaining the YAML files <exp-yaml-files>`,
now it's the right time to do it before you go on!

It's a good idea to start with a base of a configuration file and talk about the changes required to enable
Plug&Charge. The base config we use is the "config-sil-ocpp201.yaml", which already contains the configuration
for OCPP2.x.

Module Configurations
---------------------

We need to take a closer look at the configuration of the following modules:

* EvseManager
* EvseV2G
* Auth
* EvseSecurity

EvseManager
~~~~~~~~~~~

* In case of AC, make sure that `ac_hlc_enabled` is set to `true` in order to allow ISO15118 communication.
* Make sure `payment_enable_contract` is set to `true`.

EvseV2G
~~~~~~~~~~~

* Make sure `tls_security` is set to `allow` or `force`.
* Make sure `verify_contract_cert_chain` is set to `true`.

Auth
~~~~~~~~~~~

* Make sure the EvseManager module is listed as a connection of `token_provider`. This is important, because only
  in this case the authorization request including the contract certificate is actually received by the Auth module.
* Make sure the OCPP module is configured as the single `token_validator`.

EvseSecurity
~~~~~~~~~~~~

Please refer to :ref:`Documentation of the EvseSecurity module <everest_modules_handwritten_EvseSecurity>` 
for information on the ISO15118 configuration. It describes how to configure the paths to the required certificates and keys.

.. _how-to-configure-pnc-ocpp-configuration:

OCPP 1.6 and OCPP 2.x configuration
-----------------------------------

For a general introduction to how to configure OCPP in EVerest, please refer to :ref:`the OCPP1.6 tutorial <tutorial-ocpp16>`
or :ref:`the OCPP2.x tutorial <tutorial-ocpp2>`.

Since Plug&Charge has been backported from OCPP 2.x to OCPP 1.6, the
configuration options to control the process are mostly identical.
These options are described in the following section, where differences
between OCPP 1.6 and OCPP 2.x are marked.

These OCPP configuration options are relevant for the Plug&Charge process:

* ISO15118CertificateManagementEnabled (bool): Global feature flag to enable
  certificate management using ISO15118. This enables the ISO15118 message handling
  via the DataTransfer mechanism according the the OCPP1.6 Plug&Charge Whitepaper.
  (only required for OCPP1.6, OCPP2.x does not require this option). This option
  should be set to `true` in order to allow certificate management for Plug&Charge.
* ISO15118PnCEnabled (bool): Global feature flag to enable authorization using 
  contract certificates. This option should be set to `true`.
* CentralContractValidationAllowed (bool): If enabled and charging station can
  not validate the contract locally (e.g. because no MO root certificate is
  installed), the charging station provides the contract certificate as part
  of the Authorize.req so that the CSMS can verfiy the contract instead.
* ContractValidationOffline (bool): If enabled, the charging station will try
  to validate a contract certificate when it is offline using the authorization
  cache or the local authorization list. If this is set to `false`, Plug&Charge
  will fail if the charging station is offline.
* ISO15118Ctrlr::V2GCertificateInstallationEnabled (bool, only OCPP2.x):
  Allows the CSMS to install an SECC leaf certificate on the charging station.
  This must be enabled in case the charging station shall receive the SECC leaf
  certificate from the CSMS.
* ISO15118Ctrlr::ContractCertificateInstallationEnabled (bool, only OCPP2.x):
  Allows contract certificate installation installtion/update in the EV
  via ISO15118.

The following configuration options control parameters of the certificate
signing request that is initiated by the charging station automatically in case
Plug&Charge is enabled and no (valid) SECC Leaf Certificate is currently installed.

* SeccLeafSubjectCommonName (string, ISO15118Ctrlr::SeccId in OCPP 2.x)
* SeccLeafSubjectCountry (string, ISO15118Ctrlr::CountryName in OCPP 2.x)
* SeccLeafSubjectOrganization (string, ISO15118Ctrlr::OrganizationName in OCPP 2.x)

These configuration keys can be configured manually or controlled by the CSMS according to its needs. If the CSMS rejects the CSR
from the charging station or does not return a certificate after the specified timeouts and retries, it is likely that the values
of these configuration keys do not match the expectations of the CSMS. Contact your CSMS partner in this case.

----

**Authors**: Piet Gömpel
