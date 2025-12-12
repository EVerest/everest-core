.. _htg_eichrecht:

#########
Eichrecht
#########

It is recommended to use a power meter that has field-proven support for
Eichrecht implementation. The power meter should support tracking of the
transactions and signing of the OCMF packets.

EVerest transports the signed meter values from the power meter to the
OCPP CSMS and triggers the start and stop of a transaction. It does not
create, store or modify the signed meter values.

Requirements for power meter hardware and EVerest driver:

-  EVerest provides information according to OCMF standard in the
   *start_transaction* and *stop_transaction* commands as described
   here: https://github.com/SAFE-eV/OCMF-Open-Charge-Metering-Format
   
-  After power failure of the complete system, an ongoing transaction
   before power failure shall be closed properly (including the signed
   meter value) in the CSMS. For this to work, the *stop_transaction*
   needs to be implemented correctly. On startup, the EvseManager will
   call *stop_transaction(last_uuid)* to try to close the ongoing
   transaction. The power meter driver shall return the signed meter
   value for the transaction and close it. After that, it will call
   *stop_transaction("")* with an empty argument. Then, the power meter
   driver shall clear all pending transactions in the power meter, if
   any.

-  If the communication between EVerest driver and the power meter is
   lost and re-established during a charging session, the charging
   session shall still receive the signed meter value normally at the
   end of the session.

-  EvseManager should be configured to stop the charging session on
   power meter failures (ensure that fail_on_powermeter_errors is set to
   ``true``).

----

**Authors**: Cornelius Claussen, Manuel Ziegler
