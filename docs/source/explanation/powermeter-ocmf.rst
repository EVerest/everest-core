.. _exp-powermeter-ocmf:

########################
Powermeter OCMF Handling 
########################

This document explains how EVerest modules implementing the :doc:`powermeter interface </reference/interfaces/powermeter>`
shall handle OCMF report generation and transmission when used in conjunction with the
:ref:`EvseManager module <everest_modules_EvseManager>`.

The following sequence diagrams illustrate the interactions between the involved modules
during the start and stop of a transaction, including error handling scenarios:

- :ref:`Start of a transaction <exp-powermeter-ocmf-start-transaction>`
- :ref:`Stopping transaction in error <exp-powermeter-ocmf-stopping-transaction-error>`
- :ref:`Start of Powermeter or recovery after communication loss <exp-powermeter-ocmf-start-recovery>`

.. _exp-powermeter-ocmf-start-transaction:

Start of a transaction
======================

.. mermaid::

   sequenceDiagram
   autonumber
   participant Powermeter
   participant EvseManager
   participant OCPP
   participant CSMS

   title Start of a Transaction

   Note over EvseManager: User plugs in EV and authorizes

   EvseManager->>OCPP: Event(SessionStarted)

   OCPP->>CSMS: StatusNotification.req(Preparing)
   CSMS-->>OCPP: StatusNotification.conf

   alt successful case
       EvseManager->>Powermeter: startTransaction
       Powermeter-->>EvseManager: startTransaction Response (OK/ID)

       EvseManager->>OCPP: Event(TransactionStarted)
       OCPP->>CSMS: StartTransaction.req
       CSMS-->>OCPP: StartTransaction.conf

       Note over EvseManager: Transaction started successfully

   else startTransaction failing due to power loss
       EvseManager->>Powermeter: startTransaction
       Powermeter-->>EvseManager: startTransaction Response (FAIL)

       EvseManager->>OCPP: Event(Deauthorized)

       OCPP->>CSMS: StatusNotification.req(Finishing)
       CSMS-->>OCPP: StatusNotification.conf

       EvseManager->>OCPP: raiseError (PowermeterTransactionStartFailed)
       OCPP->>CSMS: StatusNotification.req(Finishing, PowermeterTransactionStartFailed)
       CSMS-->>OCPP: StatusNotification.conf

       Note over EvseManager: Transaction did not start
   end

   alt EvseManager configured to become inoperative in case of Powermeter CommunicationError
       Powermeter->>EvseManager: raise_error(CommunicationError)
       Note over Powermeter,EvseManager: Powermeter raises a CommunicationError <br/>and EvseManager is registered for notification
       EvseManager->>OCPP: raise_error (Inoperative)
       OCPP->>CSMS: StatusNotification.req(Faulted)
       CSMS-->>OCPP: StatusNotification.conf
   end

.. _exp-powermeter-ocmf-stopping-transaction-error:

Stopping Transaction in Error
=============================

.. mermaid::

   sequenceDiagram
   autonumber
   participant Powermeter
   participant EvseManager
   participant OCPP
   participant CSMS

   title Stopping Transaction in Error

   Note over Powermeter, CSMS: Transaction is running

   Powermeter->>Powermeter: detects a <br/> CommunicationError
   Note over Powermeter,EvseManager: Powermeter raises a CommunicationError <br/>and EvseManager is registered for notification
   Powermeter->>EvseManager: raise_error (CommunicationFault)
   Powermeter->>OCPP: raise_error (CommunicationFault)

   OCPP->>CSMS: StatusNotification.req(Charging, CommunicationFault)
   CSMS-->>OCPP: StatusNotification.conf

   alt EvseManager configured to become inoperative in case of PowermeterCommError
       EvseManager->>EvseManager: Pause charging
       EvseManager->>OCPP: raiseError (Inoperative)
       OCPP->>CSMS: StatusNotification.req(Faulted)
       Note over EvseManager: Note that we would just continue charging otherwise
   end

   Note over Powermeter, CSMS: User stops the transaction

   alt successful case (Powermeter has no CommunicationError)
       EvseManager->>Powermeter: stopTransaction (ID)
       Powermeter-->>EvseManager: stopTransaction Response (OK/OCMF)
       EvseManager->>OCPP: Event(TransactionFinished(OCMF))

       OCPP->>CSMS: StopTransaction.req(OCMF)
       CSMS-->>OCPP: StopTransaction.conf
   else stopTransaction failing due to subsequent power loss (this applies as well when Powermeter still in CommunicationError)
       EvseManager->>Powermeter: stopTransaction (ID)
       Powermeter->>EvseManager: stopTransaction Response (FAIL)
       EvseManager->>OCPP: Event(TransactionFinished)

       Note right of OCPP: In this case we can't stop the transaction including the OCMF
       OCPP->>CSMS: StopTransaction.req()
       CSMS-->>OCPP: StopTransaction.conf
   end

.. _exp-powermeter-ocmf-start-recovery:

Start of Powermeter or recovery after communication loss
========================================================

.. mermaid::

  sequenceDiagram
  autonumber
  participant Powermeter
  participant EvseManager

  title Start of Powermeter or recovery after communication loss

  Note over Powermeter: Device communication (re)established

  Powermeter->>Powermeter: Request status from device
  Powermeter->>Powermeter: Detects a running transaction
  Powermeter->>Powermeter: Mark need_to_stop_transaction to true

  alt Next command is startTransaction
      EvseManager->>Powermeter: startTransaction
      Powermeter-->>Powermeter: stopTransaction
      Note over Powermeter: internal triggered stopTransaction will not send <br>a response to EvseManager since no stopTransaction was issued
      Powermeter->>Powermeter: Mark need_to_stop_transaction to false
      Powermeter-->>EvseManager: startTransaction Response (OK/ID)
      Powermeter->>Powermeter: Mark need_to_stop_transaction to true

      Note over EvseManager: Transaction started successfully

  else Next command is stopTransaction
      EvseManager->>Powermeter: stopTransaction
      Powermeter-->>EvseManager: stopTransaction Response (OK/OCMF)
      Powermeter->>Powermeter: Mark need_to_stop_transaction to false
  end

  Note over Powermeter: In case of CommunicationError during start/stop<br> transaction please check the start/stop transaction diagrams

----

**Authors**: Florin Mihut, Piet Gömpel
