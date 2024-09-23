# OCPP 2.0.1: Device model initialization and inserting of config values

If there is no custom database used for the device model, and 'initialize_device_model' is set to true in the 
constructor of ChargePoint, the device model will be created or updated when ChargePoint is created. This document will
give more information about the files you need and what will be updated when the 'initialize_device_model' is set
to true.


## Database, component config and config file paths

Along with the 'initialize_device_model' flag, a few paths must be given to the constructor:
- The path of the device model migration files (normally `resources/v201/device_model_migration_files`).
- The path of the device model database.
- The path of the directory with the device model config. There should be two directories in it: 'standardized' and 
  'custom', both containing device model config.


## Component config

When the database is created for the first time, it will insert all components, variables, characteristics and 
attributes from the component config. 


## Update config values

Each time the ChargePoint class is instantiated, the component config is read and the values will be set to the database 
accordingly. Only the initial values will be set to the values in the component config. So if for example the CSMS 
changed a value, it will not be updated to the value from the component config file.


## Update component config

To update a component, just place the correct json component config in the `component_config/custom` or 
`component_config/standardized` folder. When restarting the software, it will:
- Check if there are Components in the database that are not in the component config's. Those will be removed.
- Check if there are Components in the component config's that are not in the database. Those will be added.
- Check if anything has changed inside the Component (`Variable`, `Characteristics` or `Attributes`). 
  Those will be removed, changed or added to the database as well. 
  
Note: When the `evse_id` or `connector_id` of a component is changed, this is seen as the removal of a Component and 
addition of a new one. 

Note: OCPP requires EVSE and Connector numbering starting from 1 counting upwards.

Note: There should be no duplicate components or variables in the component config files.
