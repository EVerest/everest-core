.. _everest_modules_handwritten_EvseSecurity:

====================================
EvseSecurity
====================================

:ref:`Link <everest_modules_EvseSecurity>` to the module's reference.

This module implements the `evse_security <https://github.com/EVerest/everest-core/blob/main/interfaces/evse_security.yaml>`_ interface. It wraps the functionality of the `libevse-security <https://github.com/EVerest/libevse-security>`_ to provide access to security-related operations to other Everest modules such as OCPP and ISO15118. These modules require similar security-related operations and shared access to certificates and keys, which this module facilitates. For detailed information about the provided functionality, please refer to the README within the `libevse-security <https://github.com/EVerest/libevse-security>`_.

CA Certificate Domains
=======================

The combination of ISO15118 and OCPP defines several CA certificate domains for charging stations, addressed and covered by this module:

* V2G root: Trust anchor for ISO15118 TLS communication between the charging station and the electric vehicle.
* CSMS root: Trust anchor for TLS communication between the charging station and OCPP CSMS.
* MF root: Trust anchor of the manufacturer to verify firmware updates.
* MO root: Trust anchor of the Mobility Operator domain to verify contract certificates.

Module Configuration
=====================

The following instructions describe how to configure the module parameters mainly for two domains: OCPP communication and ISO15118 communication.

Configuration for OCPP
----------------------

In OCPP and OCPP security profiles, the security level of the connection is specified as follows:

* SecurityProfile 0: Unsecure transport without Basic Authentication
* SecurityProfile 1: Unsecure transport with Basic Authentication
* SecurityProfile 2: TLS with Basic authentication
* SecurityProfile 3: TLS with client-side certificates

Only when security profiles 2 or 3 are used, the configuration of this module is relevant for OCPP communication. In this case, the charging station acts as a TLS client.

The ``csms_ca_bundle`` config parameter specifies a path to a file containing trusted CSMS root certificates. The server certificate presented by the CSMS server must be signed by one of the trusted root certificates specified in this file. If new root certificates are installed using the ``install_ca_certificate`` command with the CSMS domain specified, the new CA certificate is installed into the specified bundle and used for further validations.

Note: The OCPP modules in Everest can be configured to also trust the operating system's default verify paths. The parameter controlling this behavior is ``UseSslDefaultVerifyPaths``. If configured to ``true``, the ``csms_ca_bundle`` need not necessarily be configured.

If security profile 3 is used, the ``csms_leaf_cert_directory`` and ``csms_leaf_key_directory`` need to be configured. These parameters specify the directory where the client certificate and key for the mTLS connection are located. New client CSMS certificates can be installed using the ``update_leaf_certificate`` command with the CSMS domain specified.

Configuration for ISO15118
--------------------------

For ISO15118 communication, the charging station provides a server endpoint to which the electric vehicle connects. The communication may be secured using TLS. If TLS is required/configured, correct configuration of the ``secc_leaf_cert_directory`` and ``secc_leaf_key_directory`` is required. These directories are used to locate the server certificate and key for the ISO15118 TLS server provided by the charging station.

Private Key Password
--------------------

If private keys are generated in the process of generating a certificate signing request (CSR), the private keys are not encrypted with a password. Therefore, no password needs to be configured if all certificates are installed using the ``generate_certificate_signing_request`` and ``update_leaf_certificate`` commands. If existing certificates and private keys are to be installed, the ``private_key_password`` parameter specifies the password for encrypted private keys. Please note that only one value can be configured for possibly multiple encrypted private keys.

More about CSMS, V2G, MO and MF Bundles
---------------------------------------

* The ``v2g_ca_bundle`` is used to verify the installation of SECC leaf certificates using the ``update_leaf_certificate`` command. 
* The ``csms_ca_bundle`` is used to verify the installation of CSMS leaf certificates using the ``update_leaf_certificate`` command.
* The ``mo_ca_bundle`` is used to verify contract certificates provided by the electric vehicle as part of the ISO15118 Plug & Charge process.
* The ``mf_ca_bundle`` is used to verify firmware update files.
New root certificates can be installed in the specified domain using the ``install_ca_certificate`` command.
