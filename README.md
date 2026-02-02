
![Alt text](docs/images/everest_horizontal-color.svg)

[![OpenSSF Best Practices](https://bestpractices.coreinfrastructure.org/projects/6739/badge)](https://bestpractices.coreinfrastructure.org/projects/6739)

# EVerest

EVerest is a [Linux Foundation Energy](https://lfenergy.org/) backed open-source modular
framework for setting up a full stack environment for EV charging. The modular
software architecture fosters customizablility and lets you configure your
dedicated charging station based on interchangeable modules. All communication
is performed by the lightweight and flexible MQTT message queueing service.
EVerest will help to speed the adoption to e-mobility by utilizing all the
open-source advantages for the EV charging world. It will also enable new
features for local energy management, PV-integration and many more!

# Main Features

- Modular and extensible architecture
- Support for AC and DC charging
- Support for EV charging protocols
    - OCPP 1.6, OCPP 2.0.1 and OCPP 2.1
    - ISO 15118-2, -3 and -20
    - IEC 61851
    - DIN SPEC 70121
- Ready-to-use hardware drivers for many compatible hardware components 
    - BSPs for charge controllers
    - Powermeters
    - Isolation monitors
    - DC Power supplies
    - RFID/NFC readers
    - Payment terminals
- Energy management implementations and API
- Standardized and stable APIs to allow easy integrations
- Bring-up modules for custom hardware testing and integration
- Ensured standards compliance
- OTA service to keep EV chargers up-to-date
- Security best practices following OpenSSF
- ISO / IEC 5230 open source license compliance
- Secure communication channels through TPM
- Yocto support for custom embedded Linux images

# Getting Started

See our documentation on [everest.github.io](https://everest.github.io) to get started with using or developing EVerest. The best
way to start is with our [Getting Started Guides](https://everest.github.io/nightly/how-to-guides/getting-started/index.html).

# Community

Find more information about the community and ways to get involved [here](https://everest.github.io/nightly/project/community.html).
# Contributing to EVerest

Anyone can contribute to EVerest! Learn more about getting involved
[here](https://everest.github.io/nightly/project/contributing.html).

# License

EVerest is licensed under the Apache License, Version 2.0.
See [LICENSE](LICENSE) for the full license text.

# Background

The EVerest project was initiated by PIONIX GmbH to help with the
electrification of the mobility sector.
