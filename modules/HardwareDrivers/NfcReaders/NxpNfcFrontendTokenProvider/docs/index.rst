.. _everest_modules_handwritten_NxpNfcFrontendTokenProvider:

***************************
NxpNfcFrontendTokenProvider
***************************

The module ``NxpNfcFrontendTokenProvider`` implements the ``auth_token_provider`` interface.
It reads data from NXP NFC frontend chips like CLRC663.
It relies on **NxpNfcRdLib_Linux_v07.10.00_PUB.zip** which users need to obtain from NXP: [nxp.com](https://www.nxp.com/design/design-center/development-boards-and-designs/nfc-reader-library-software-support-for-nfc-frontend-solutions:NFC-READER-LIBRARY).
The variety of hardware supported by ``NxpNfcFrontendTokenProvider`` is limited by the time of writing but could be extended by modification of the ``nxpnfcrdlib_wrapper``.

Building the module
===================
The module can be built in two ways:

* Setting *CMake* variable ``EXTERNAL_NXPNFCRDLIB_ZIP_PATH_NxpNfcFrontendTokenProvider`` to the path to ``NxpNfcRdLib_Linux_v07.10.00_PUB.zip``.
* Leaving this option undefined uses the included ``NamedPipeDataSource`` which acts as a drop-in replacement without any external build dependencies but does not support real NFC hardware.

To enable users who do not use NxpNfcFrontend to build EVerest without having to explicitly set variables or options, the variable is undefined by default.

NamedPipeDataSource
-------------------

Cannot interface real NFC hardware.
During runtime it creates a named pipe (FIFO) at ``/tmp/EV_NXP_NFC_FRONTEND_TOKEN_PROVIDER_FIFO_SUBSTITUTE`` and tries to read from it.
As soon, as it can read a ``\n``-terminated line, it tries to parse it as ``<protocol>:<uid>`` (see examples below).

Configuring the module
======================

Runtime configuration allows to

* *token_debounce_interval_ms*: select a minimum intervall between publishes

See ``manifest.yaml`` for details.

Testing the module
==================

The module does not implement any commands.

Using NamedPipeDataSource
-------------------------

Possible input for ISO14443 (MIFARE card) emulation:

  .. code-block:: bash

    echo "ISO14443:44332211" > /tmp/EV_NXP_NFC_FRONTEND_TOKEN_PROVIDER_FIFO_SUBSTITUTE
    
Possible input for ISO15693 (Vicinity card) emulation:

  .. code-block:: bash

    echo "ISO15693:8877665544332211" > /tmp/EV_NXP_NFC_FRONTEND_TOKEN_PROVIDER_FIFO_SUBSTITUTE

It publishes an ``types::authorization::ProvidedIdToken`` to the topic ``everest/tokenprovider/main/var``.

It publishes whenever the NFC chip is able to detect a RFID card of the supported types.

``Published JSON:``

  .. code-block:: JSON
 
        {
            "data": {
                "authorization_type" : "RFID",
                "id_token" : {
                    "type" : "ISO14443",
                    "value" : "74F2EF5B"
                }
            },
            "name" : "provided_token"
        }


Using NxpNfcFrontendWrapper
---------------------------

Requires NFC hardware from NXP.
A hardware setup similar to the one shown in the ``nxpnfcrdlib_wrapper`` documentation can be used.
Detection of an RFID card of the supported type will trigger publishing of a ``types::authorization::ProvidedIdToken``.
