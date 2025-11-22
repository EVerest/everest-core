.. _everest_modules_handwritten_evse_manager_consumer_API:

*******************************************
evse_manager_consumer_API
*******************************************

:ref:`Link <everest_modules_evse_manager_consumer_API>` to the module's reference.

See ``doc/everest_api_specs/evse_manager_consumer_API/asyncapi.yaml`` for a full AsycAPI specification.

Session Info
=============

This API module additionally provides a ``SessionInfo`` class to represent information about EVSE sessions.
The data for ``SessionInfo`` is not simply forwared from the internal EVerest representation. The internal
represenation is processed and converted to the external API representation.