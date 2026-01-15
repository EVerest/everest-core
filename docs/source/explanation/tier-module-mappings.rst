.. _tier_module_mapping:

**********************
3-tier Module Mappings
**********************

EVerest modules and even individual interface implementations can have mappings
assigned to them. These mappings are inspired by the OCPP 3-tier model and are
available for error handling since `everest-framework v0.16.0 <https://github.com/EVerest/everest-framework/releases/tag/v0.16.0>`_,
which is included in everest-core since `release 2024.7.0. <https://github.com/EVerest/everest-core/releases/tag/2024.7.0>`_.

These mappings are exposed for usage in module code since `everest-framework v0.18.0 <https://github.com/EVerest/everest-framework/releases/tag/v0.18.0>`_,
which is included in everest-core since `release 2024.10.0. <https://github.com/EVerest/everest-core/releases/tag/2024.10.0>`_.

Following an example how a mappping for the EvseManager could look like:

.. code-block:: yaml

    connector_1:
      module: EvseManager
      mapping:
        module:
          evse: 1
          connector: 1

This would result in a mapping of the whole module,
including its implementations for e.g. evse and token_provider to "evse = 1"
and "connector = 1".

By default, a module is mapped to the whole charging station.
So to ensure that only the parts of the module that should belong
to a specific evse/connector are actually mapped to it,
you could replace this simple mapping with a more detailed one
as shown in the following example:

.. code-block:: yaml

    connector_1:
      module: EvseManager
      mapping:
        implementations:
          evse:
            evse: 1
            connector: 1

Here, the module stays mapped to the whole charging station
and therefore an implementation as well. For the "evse" implementation,
this mapping is now overwritten to indicate that it belongs to
a specific "evse = 1" and "connector = 1".

Modules can access the mapping information in the following ways depending
on which specific information is required.

If the mapping of a requirement is of interest it can be accessed via a
get_mapping() function:

.. code-block:: cpp

    r_name_of_the_requirement->get_mapping()

This returns an optional Mapping struct.

If the mapping of an interface implementation is of interest it can
also be accessed via a get_mapping() function:

.. code-block:: cpp

    p_name_of_an_implementation->get_mapping()

This returns an optional Mapping struct.

If the mapping of the current module is of interest it can be accessed via the
module info:

.. code-block:: cpp

    this->info.mapping

This returns an optional Mapping struct.

Mapping information is also available in error reporting via
"error.origin.mapping":

.. code-block:: cpp

    const auto error_handler = [this](const Everest::error::Error& error) {
        const auto evse_id = error.origin.mapping.has_value() ? error.origin.mapping.value().evse : 0;
    };

    const auto error_cleared_handler = [this](const Everest::error::Error& error) {
        const auto evse_id = error.origin.mapping.has_value() ? error.origin.mapping.value().evse : 0;
    };

    subscribe_global_all_errors(error_handler, error_cleared_handler);

