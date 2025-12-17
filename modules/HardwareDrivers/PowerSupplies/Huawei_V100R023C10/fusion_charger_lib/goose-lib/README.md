# Goose library

Provides:
- Ethernet interface for macOS and Linux
  - note that macOS implementation is not really tested; Linux's implementation is mostly based on Huawei's FusionCharger documentation
- Ethernet frame abstraction
- Goose Frame abstraction
  - contains Goose PDU abstraction which in turn contains APDU BER encoded data, which is partly abstracted

## Build and test

This library is built and tested as part of the build process of everest-core.
