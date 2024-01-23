# Interaction with EVSE Manager

This module sets callbacks into libocpp to receive `ChangeAvailability.req` updates from the CSMS.

These are sent to the EVSE Manager in `enable_disable` commands with a priority of 5000. ('types/energy_manager.yaml' contains the valid range.)

5000 is mid-range.