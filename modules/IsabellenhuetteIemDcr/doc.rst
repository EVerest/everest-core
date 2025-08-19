.. _everest_modules_handwritten_IsabellenhuetteIemDcr:

..  This file is a placeholder for an optional single file
    handwritten documentation for the IsabellenhuetteIemDcr module.
    Please decide whether you want to use this single file,
    or a set of files in the doc/ directory.
    In the latter case, you can delete this file.
    In the former case, you can delete the doc/ directory.
    
..  This handwritten documentation is optional. In case
    you do not want to write it, you can delete this file
    and the doc/ directory.

..  The documentation can be written in reStructuredText,
    and will be converted to HTML and PDF by Sphinx.

*******************************************
IsabellenhuetteIemDcr
*******************************************

Module implements Isabellenhuette IEM-DCR power meter driver, connecting via HTTP/REST.

Implementation details
===========

This section offers some additional information on driver implementation. The underlying HTTP communication functionality
is mainly duplicated from other open source powermeter modules of EVerest to support a standarization of this interface
later on.

Initialization
------------
It begins with checking some plausibility measures on the handed configuration. Its default values are given in manifest.yaml.
Please make sure to explicitly specify values that deviate from default configuration before starting the driver. If there is no
conspicuousness in configuration, HTTP communication is verified with GET requests on /gw node. In case of no success, several
retries are performed (as specified in config). On success POST /gw is issued for transfering CI, CT and datetime to IEM-DCR. 
Please note, that issuing POST /gw is only possible once after IEM-DCR power-up. So CI and CT are frozen until next power-cycle
and datetime will be automatically updated using another node (POST /datetime) in configurable intervals. Therefore a warning
will appear on EVerest console if CI and CT are already written and could not be updated. After this procedure the initial tariff
text is transferred as configured. This will show up on display before a charging transaction.

Live values
------------
Each second the MQTT variable Powermeter is updated to current values of /metervalue node. Also the public key is made available
via MQTT.

Start transaction
------------
Starting a transaction will terminate any other running transaction (if there is one). The status type TransactionRequestStatus::
NOT_SUPPORTED is returned, if given evse_id does not match CI (which was already transfered in initialization phase) and if IEM-DCR
is in error state. Please refer to retrurned TransactionStartResponse.error for distinguishing between them. Starting a charging
transaction will engage POST /user and POST /receipt. Please note that IEM-DCR automatically handles signed data tuple pagination. So
the only place for transaction id defined by the charging station is the OCMF ID attribute. It will be filled from this driver with  
TransactionReq.identification_data. If this optional attribute is not given or empty, TransactionReq.transaction_id will be used
instead. Please note that a transaction cannot be started while the sensor unit detects a current above activation treshold.
Please refer to operation manual for details.

Stop transaction
------------
If a transaction is in progress, it will be stopped and its signed data tuple returned. If no transaction is running, the last signed
data tuple will be returned. Therefore input parameter transaction_id of this routine has no impact on its operation. Please note that
TransactionRequestStatus::UNEXPECTED_ERROR may be returned, if no transaction is in progress and there has also been no transaction 
before. Please also note that a transaction cannot be stopped while the sensor unit detects a current above activation treshold.
Please refer to operation manual for details.

References
============
`IEM-DCR-125 <https://www.isabellenhuette.com/de/loesungen/produkte/iem-dcr-125>`_
`IEM-DCR-1000 <https://www.isabellenhuette.com/de/loesungen/produkte/iem-dcr-1000>`_
`IEM-DCR-1500 <https://www.isabellenhuette.com/de/loesungen/produkte/iem-dcr-1500>`_
