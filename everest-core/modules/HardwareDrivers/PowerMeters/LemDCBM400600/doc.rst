.. _everest_modules_handwritten_LemDCBM400600:

..  This file is a placeholder for an optional single file handwritten documentation for
    the LemDCBM400600 module.
    Please decide weather you want to use this single file,
    or a set of files in the doc/ directory.
    In the latter case, you can delete this file.
    In the former case, you can delete the doc/ directory.

..  This handwritten documentation is optional. In case
    you do not want to write it, you can delete this file
    and the doc/ directory.

..  The documentation can be written in reStructuredText,
    and will be converted to HTML and PDF by Sphinx.

*******************************************
LEM DCBM 400/600
*******************************************

:ref:`Link <everest_modules_LemDCBM400600>` to the module's reference.
Module implementing the LEM DCBM 400/600 power meter driver adapter via HTTP/HTTPS.


Description
===========

The module consists of a single ``main`` implementation that serves the ``powermeter`` interface. Requests/commands
to the meter are translated and forwarded to the device via HTTP/HTTPS.


Initialization
------------

On module initialization, the driver fetches the device's metric id from the  ``/v1/status`` api. Consequently, this also ensures
connectivity to the device.
The initialization will fail (with a thrown exception) in case this cannot be established (possibly after a limited amount of retries).

Furthermore, at initialization the initial time sync setup is scheduled after a 2 minute waiting time (which is then executed
during the module's "ready" thread loop), cf. also the notes on time synchronization below.

Variable Powermeter
------------

Publication of the ``powermeter`` var is done with approx. frequency 1/second. This fetches the current ``livemeasure``
values from the device's ``/v1/livemeasure`` endpoint and injects the meter id as determined at initialization.

Command start_transaction
------------

A ``start_transaction`` command is directly forwarded via a ``POST``  to the ``/v1/legal`` endpoint with a copy of the transaction request
as payload (up to renaming of attributes). It returns ``true``, if the device (possibly after a limited amount of retries) returns a success
response with a valid payload that indicates a ``running`` transaction status, otherwised it returns ``false``.


Command stop_transaction
------------

A ``stop_transaction`` command  results into two requets to the devie.

First, a ``PUT`` to the ``/v1/legal`` endpoint stops the transaction.

Then, a call to the ``/v1/ocmf/`` endpoint fetches the OCMF report for the provided transaction id. Note that this always
fetches the report of the `last` transaction with this id (in case if multiple transactions with the same id had been
running).

If both requests are successful (possibly after a limited amount of retries), the returned OCMF string is forward 1:1.

In case of an error, an empty string is returned.


Module Configuration
===========

The module has the following configuration parameters:

ip_address
------------
IP address (or DNS/Host name) of the device.

port (optional)
------------
Port used to reach the device. Defaults to ``80``. Note that the default value of ``80`` is used independent on whether
TLS is enabled or not (which is in coherence with the device`s behavior).

meter_tls_certificate (optional)
------------
The meter's TLS X.509 certificate in PEM format. If provided, TLS will be used for communication with the device. See
:ref:`notes on TLS <TLS Notes>` below.


NTP Settings (optional)
------------

If NTP servers are supposed to be used for time sync by the device,
those can provided via
- ``ntp_server_1_ip_addr``, ``ntp_server_1_port`` for the first NTP server, and
- ``ntp_server_2_ip_addr``, ``ntp_server_2_port`` for the first NTP server.

If the first server is provided, NTP will be activated on module initialization. Otherwise, a
regular time sync with the system time will be executed.

Note that the wording "ip_address" follows the operational manual (cf. 4.2.3. of the `Communication protocols manual`, see references below).
However, according to this manual DNS names are allowed, too.




Resilience Settings (optional)
------------
The following optional settings may be set to adapt the resilience behavior behavior of the module:

- ``resilience_initial_connection_retries`` and ``resilience_initial_connection_retry_delay`` define the number of attempted
retries and delay inbetween in  milliseconds in case of an error (failed connection or unexpected response from the device) during the module
initialization. This potentially delays module initialization, but may prevent a module failure at startup (e.g., if the device
is not ready yet).
- ``resilience_transaction_request_retries`` and ``resilience_transaction_request_retry_delay`` similarly
define the according values but for connection attempts during a transaction start or stop command handling.
In order to prevent a greater command return delay (and since the device is assumed to be set up and running when
transactions are started), default values are considerably lower than the ones for initialization.



Notes
===========

Time Sync
------------

The powermeter device needs to be regularly time synced in order to function properly
(cf.
The module is capable of performing regular syncs with the system time, or -- alternatively --
allows to setup NTP servers (cf. the configuration parameters above).

If no NTP server is provided, a sync right before each transaction start is ensured in order to
allow for the maximum possible transaction duration of 48 hours. Cf. the `Operation Manual` section 7.8.1 for
more details.

Also note the device's manual suggests a start-up time of 2 minutes before settings (such as
time sync) should be persisted (cf. the `Communication protocols manual` section 4).
This is payed regard to in the module.

Error Handling / Resilience
------------

In general responses are checked for a valid response code and body. In case of validation errors or an http error,
requests are retried to provide some resilience.

For the initialization requests, 25 retry attempts are made with a 10 second delay.
For start/stop transaction requests, 3 retry attempts with a 200ms delay are made.


.. _TLS Notes:

TLS Notes & Limitations
------------

The device brings its own self-signed certificate. Since there is no manufacturer root CA, this certificate must be provided
in order to establish a reasonable TLS connection. Note that the provided certificate uses a private key of 1024bit length, which
in general is considered vulnerable.

..  code-block:: bash

  curl 'http://<DEVICE ADDRESS>:<DEVICE PORT>/v1/certificate'

TLS can be enabled via:

..  code-block:: bash

  curl --location --request PUT 'https://<DEVICE ADDRESS>:<DEVICE PORT>/v1/settings' \
  --header 'Content-Type: application/json' \
  --data '{
      "http": {
          "tls_on": true
      }
  }'

References / Links
============
- `Official product page https://www.lem.com/en/dcbm-400-600 <https://www.lem.com/en/dcbm-400-600>`_
- `Operation Manual <https://www.lem.com/en/file/10314/download>`_
- `Communication protocols manual <https://www.lem.com/en/file/11215/download>`_
