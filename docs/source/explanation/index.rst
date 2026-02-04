###########
Explanation
###########

The explanation pages will give you detailed information about the features of
EVerest.

Let us have a look at the most important topics first.
Below that, you will be presented with a categorized list of all articles.

.. grid:: 1 2 2 3
   :gutter: 2

   .. grid-item-card:: Framework Overview
      :link: high-level-overview
      :link-type: doc

      Get a high-level overview of the EVerest framework.

   .. grid-item-card:: Error Framework
      :link: error-framework
      :link-type: doc

      How to communicate error states between modules.

   .. grid-item-card:: EVerest Modules in Detail
      :link: detail-module-concept
      :link-type: doc

      Learn about the module concept of EVerest.

   .. grid-item-card:: Tier Module Mapping
      :link: tier-module-mappings
      :link-type: doc

      EVerest's 3-tier module mapping explained.

   .. grid-item-card:: Adapt EVerest
      :link: adapt-everest/index
      :link-type: doc

      Learn how EVerest can be adapted to your use-case.

   .. grid-item-card:: The EVerest Dependency Manager
      :link: dev-tools/edm
      :link-type: doc

      Tool helping to orchestrate dependencies between the different EVerest repositories.

   .. grid-item-card:: The ev-cli Development Tool
      :link: dev-tools/edm
      :link-type: doc

      Command line tool to generate C++ code from interface and manifest definitions.

   .. grid-item-card:: The Plug&Charge Process in EVerest
      :link: dev-tools/edm
      :link-type: doc

      Learn how Plug&Charge is implemented in EVerest.

   .. grid-item-card:: Linux / Yocto and EVerest
      :link: linux-yocto/index
      :link-type: doc

      Learn how to integrate EVerest in your embedded application via Yocto and allow for secure OTA updates.

   .. grid-item-card:: Hardware Architecture
      :link: hardware-architecture
      :link-type: doc

      Some ideas and guidance on the general architecture of AC or DC chargers.

   .. grid-item-card:: Powermeter OCMF Handling
      :link: powermeter-ocmf
      :link-type: doc

      How OCMF records are expected to be handled by modules implementing powermeters.

   .. grid-item-card:: A Selection of included Hardware Drivers
      :link: hardware-drivers
      :link-type: doc

      Description of natively supported hardware driver modules included in EVerest.

   .. grid-item-card:: Structure of the EVerest Documentation
      :link: the-everest-documentation
      :link-type: doc

      How this documentation is structured.

.. toctree::
    :hidden:
    :maxdepth: 1

    high-level-overview
    error-framework
    detail-module-concept
    tier-module-mappings
    adapt-everest/index
    energymanagement/index
    dev-tools/edm
    dev-tools/ev-cli
    pnc-process
    linux-yocto/index
    hardware-architecture
    hardware-drivers
    the-everest-documentation
    powermeter-ocmf
