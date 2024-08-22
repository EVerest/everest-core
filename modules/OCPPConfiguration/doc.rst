*******************************************
OCPPConfiguration
*******************************************

This module reacts to OCPP configuration change requests.
It does so by writing the requested changes to a user-config file.
This requires a mapping between OCPP variable names and the corresponding configuration file entries.
The configuration file is specified in the manifest file of the module (see :ref:`_mapping_file_path`).

Configuration Options
----------------------

The following configuration options are available:

.. _user_config_path:

**user_config_path**
  - **Description**: Path to the user configuration file.

.. _mapping_file_path:

**mapping_file_path**
  - **Description**: Path to the mapping file.