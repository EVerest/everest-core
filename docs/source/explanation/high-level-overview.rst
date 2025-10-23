.. exp_high_level_overview:

##############################
High-Level Overview of EVerest
##############################

EVerest an open source software stack for electric vehicle charging stations. It provides all necessary
functionalities to run a charging station, from low-level hardware interaction to high-level protocols
like OCPP and ISO15118.

Key features of EVerest
========================

All in all, you can expect the following:

* Modular and extensible architecture
* Support for AC and DC charging
* Support for EV charging protocols
    * OCPP 1.6, OCPP 2.0.1 and OCPP 2.1
    * ISO 15118-2, -3 and -20
    * IEC 61851
    * DIN SPEC 70121
* Ready-to-use hardware drivers for many compatible hardware components 
    * BSPs for charge controllers
    * Powermeters
    * Isolation monitors
    * DC Power supplies
    * RFID/NFC readers
    * Payment terminals
* Energy management implementations and API
* Standardized and stable APIs to allow easy integrations
* Bring-up modules for custom hardware testing and integration
* Ensured standards compliance
* OTA service to keep EV chargers up-to-date
* Security best practices following OpenSSF
* ISO / IEC 5230 open source license compliance
* Secure communication channels through TPM
* Yocto support for custom embedded Linux images

EVerest Architecture
=====================

EVerest contains a rich set of modules that can be combined to build a full EV charging station software stack.
The architecture is modular and based on loosely coupled components that communicate via MQTT.

.. image:: images/0-2-everest-top-level-diagram-module-communication.png

Each module runs as an independent process and communicates with other modules via well-defined interfaces.
This allows module to subscribe to variables published by other modules and to call commands provided by other modules.

A more detailed explanation of the EVerest architecture and module concept can be found in the
explaination about :doc:`EVerest modules in detail </explanation/detail-module-concept>`.


Hardware Requirements
=============================

EVerest can run on any Linux-based operating system that comes with the required dependencies.
Our (strong) recommendation is Yocto.
Find more information on how to set up your Yocto-based environment in the respective
:doc:`TODO EVerest Yocto Guide </how-to-guides/everest-yocto-setup>`.

The hardware requirements to run EVerest very much depend on the use case and the modules
that are used in the specific scenario. As a general guideline, the following minimum
requirements should be met:

* CPU: minimum imx6ULL or comparable, recommended is imx93 or AM62x
* RAM: minimum of 512 MB, recommended is 1 GB or more
* Flash: minimum of 1 GB, recommended is 4 GB or more

Getting Started
=====================

Please refer to the :doc:`Quick Start Guides </explanation/quick-start-guides>` to get started with EVerest.
