.. _everest_modules_handwritten_evse_manager_consumer_API:

.. *******************************************
.. evse_manager_consumer_API
.. *******************************************

The complete API specification can be found in the

``docs/source/reference/EVerest_API/evse_manager_consumer_API.yaml``

file in the source repository, or in the `AsyncAPI HTML documentation <../../../api/evse_manager_consumer_API/index.html>`_ automatically generated from it.

Session Info
=============

This API module additionally provides a ``SessionInfo`` class to represent information about EVSE sessions.
The data for ``SessionInfo`` is not simply forwarded from the internal EVerest representation. The internal
represenation is processed and converted to the external API representation.
