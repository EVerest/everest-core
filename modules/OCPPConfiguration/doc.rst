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

**user_config_file_name**
  - **Description**: File name of the user configuration file.
                     This should match the name of the loaded config.
                     When an ocpp configuration request is successfully handled this module will write the configuration
                     parameters into this file in the installation directory. If this file shouldn't exist it will be
                     automatically created.

.. _mapping_file_path:

**mapping_file_name**
  - **Description**: Name of the mapping file.
                     This file has to be in the `mappings` directory of this module which will be installed.