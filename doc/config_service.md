# Config Service in EVerest

WIP design document for configuration service in EVerest

## Problem description
* It is currently not possible for EVerest modules to change the configuration parameters of other modules at runtime (only via interface commands)
* Establishing this via interface commands does not scale
* OCPP2.0.1 defines the mutability of configuration parameters and requires these variables to be changed at runtime
* OCPP2.0.1 defines data structures for components and variables that are not in line with the current definition in EVerest

## Goal
* Introduce a configuration service in EVerest that allows to change and retrieve configuration parameters of EVerest modules and runtime
* Provide access to priviliged modules like OCPP to this config service 

## Requirements
* Responsible for read and write operations of configuration parameters of EVerest modules at runtime (and startup?)
* Needs to be accessed by priviliged modules (e.g. OCPP)
* Needs to notify priviliged modules (e.g. OCPP) if a variable has changed (e.g. internally or by some other module/UI etc.)

## Architecture Design

## Questions
* What is the underlying storage implementation for configuration parameters of everest modules? EVerest config + EVerest user config?
* Do we need to exclude the SQLite migration support from libocpp?

## Next steps
* Move device model storage implementation to everest-core
* Allow definition of externally managed variables in component schemas
* Allow definition of R/W configuration parameters in module manifests
* Implement config service in everest-framework

# OCPP2.0.1 Device Model implementation
* Pull device model implementation out of libocpp to be able to integrate this with EVerest --> Increases coupling of libocpp and everest-core
* Device Model implementation will be moved to OCPP2.0.1 module
* Differentiation between internally and externally managed variables
  * Internally Managed: Owned, stored and accessed by OCPP2.0.1 module in device model storage
  * Externally Managed: Owned, stored and accessed via EVerest config service
  * For externally managed variables a mapping to the EVerest configuration parameter needs to be defined!
* Add property for internally or externally managed to each variable in the component schemas
* This design allows for singe source of truth --> OCPP is source for internally managed variables and config service for externally managed configuration variables 
