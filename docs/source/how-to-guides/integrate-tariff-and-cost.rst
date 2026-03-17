.. _htg_integrate_tariff_and_cost:

#########################
Integrate Tariff and Cost
#########################

This guide explains how to subscribe to tariff and session cost updates
published by EVerest, and how the data flow differs between OCPP 1.6 and
OCPP 2.x.

*************
Prerequisites
*************

- An EVerest deployment with an OCPP module (``OCPP`` for 1.6 or ``OCPP201``
  for 2.x) and the ``Auth`` module and the relevant API modules (see next section).
- An API client that connects via MQTT and subscribes to the relevant
  consumer API topics.

**********************
Relevant Consumer APIs
**********************

Two consumer APIs carry tariff and cost information:

- **Session cost consumer API** — provides the ``tariff_message`` and
  ``session_cost`` variables on the ``session_cost`` interface.
- **Auth consumer API** — provides ``token_validation_status``, which may
  include tariff messages received at authorization time.

At least the session cost consumer API must be subscribed to receive tariff and cost
updates.

See the AsyncAPI reference for the `auth_consumer_API <../reference/api/auth_consumer_API/index.html>`_
and the `session_cost_consumer_API <../reference/api/session_cost_consumer_API/index.html>`_ for message schemas.


.. note::
    
    The OCPP implementations follow the requirements of the OCA California
    Pricing Whitepaper. Note that this whitepaper contains requirements for
    displaying pricing information to the user, so in order to comply with it,
    the tariff and session cost messages must be used to show the required
    information on a charger display. The APIs only provide the necessary data.

******************
OCPP configuration
******************

If you plan to use session cost information from OCPP (e.g. according to the California
Pricing requirements), you need to configure the OCPP modules to receive tariff and
cost information from the CSMS.

OCPP 1.6
=========

Enable the ``CostAndPrice`` profile in the OCPP 1.6 module configuration.
Once enabled, the OCPP module registers the tariff message and session cost
callbacks. Configure the configuration keys of the ``CostAndPrice`` profile
as needed.

OCPP 2.x
========

Enable the ``TariffCostCtrlr`` in the OCPP 2.x device model. The
component variables ``TariffEnabled`` and ``CostEnabled`` control which
features are active:

- **TariffEnabled** — publishes tariff / pricing messages via the
  ``tariff_message`` variable.
- **CostEnabled** — publishes running and final session cost via the
  ``session_cost`` variable.

Both can be enabled independently. Also configure other variables of the
``TariffCostCtrlr`` as needed.

.. note::

    Additional requirements of the OCPP 2.1 specification with respect to
    tariff and costs have not yet been implemented. The implemented features
    can still be used with OCPP 2.1.

*******************
Tariff message flow
*******************

Tariff messages contain human-readable pricing text in one or more
languages. They are published on the ``tariff_message`` variable of the
session cost consumer API.

OCPP 1.6
=========

Tariff messages originate from a ``SetUserPrice`` DataTransfer message sent
by the CSMS. The CSMS typically sends this message shortly after the
``Authorize.conf``, so the tariff message is available close to
authorization time.

If the ``SetUserPrice`` DataTransfer is not received within the configured
timeout, a default tariff message (if configured) is published instead.

OCPP 2.x
========

Tariff messages are published at two points:

1. **Authorization time** — from the ``personalMessage`` field in the
   ``AuthorizeResponse``. The tariff message is also included in the
   ``messages`` field of the ``token_validation_status`` on the auth
   consumer API. At this point the ``identifier_type`` is ``IdToken`` and
   the ``identifier_id`` is the token value, since no transaction exists
   yet.

2. **During the session** — from the ``updatedPersonalMessage`` field in
   ``TransactionEventResponse`` messages. At this point the
   ``identifier_type`` is ``TransactionId`` and the ``identifier_id`` is
   the OCPP transaction ID.

.. note::

   To correlate auth-time tariff messages (keyed by ``IdToken``) with
   in-session updates (keyed by ``TransactionId``), use the
   ``token_validation_status`` message on the auth consumer API to capture
   the token, then match it against the session information from the EVSE
   manager consumer API.

*****************
Session cost flow
*****************

Session cost messages contain the accumulated cost of a charging session,
broken down into cost chunks (energy, time, flat fees), with current and
next-period pricing. They are published on the ``session_cost`` variable of
the session cost consumer API.

OCPP 1.6
=========

Session cost updates are triggered by ``RunningCost`` and ``FinalCost``
DataTransfer messages from the CSMS.

OCPP 2.x
========

Session cost updates are triggered by:

- ``totalCost`` in a ``TransactionEventResponse`` — publishes a running
  cost update with status ``Running`` or ``Idle``.
- ``CostUpdated`` message — publishes a final cost update with status
  ``Finished``.

When both ``totalCost`` and ``updatedPersonalMessage`` are present in the
same ``TransactionEventResponse``, the personal message appears in both the
``tariff_message`` variable and the ``message`` field of the
``session_cost`` record.

*****************************
Relationship between the APIs
*****************************

.. list-table::
   :header-rows: 1
   :widths: 30 35 35

   * - Event
     - Auth consumer API
     - Session cost consumer API
   * - Token authorized with tariff
     - ``token_validation_status`` includes ``messages``
     - ``tariff_message`` published
   * - In-session tariff update
     - —
     - ``tariff_message`` published
   * - Running / final cost
     - —
     - ``session_cost`` published (may include ``message``)
