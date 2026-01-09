.. _htg_getting_started_hw:

#########################
Get started with Hardware
#########################

There are different approaches for getting started with running EVerest
on hardware. For each approach we have collected some starting points
and best practices to help you get up and running with EVerest.

For each of the following starting scenarios, you can find more detail
in the sections below:

-  **Using an EVerest-compatible development kit and add the BSP.** This
   is a great path to start learning about the benefits of EVerest. A
   device including a deployed EVerest image can let you experience
   charging in a more practical manner moving beyond software simulation.

-  **Start with your own hardware.** This is a less out-of-the-box
   start, but a more direct path to your own system environment. We will
   explain how to get there below.

Using an EVerest-compatible development kit
===========================================

The easiest way to get started with hardware is to use one of the development kits.
They can charge a real car out of the box and you can evaluate all features of
EVerest before building your own product.

Additionally, they come with a ready-to-use Yocto image with RAUC OTA updates, OCPP,
ISO 15118 and all other features of EVerest.

They will help you to:

* parallelize HW and SW developments for new charger projects,
* test OCPP backends (CSMS) against EVerest,
* explore new charging algorithms without the need of doing all the groundwork and
* rapidly integrate EV charging with other applications.

Currently, there are three development kits available. Choose the one that matches 
your product the closest:

1. **AC all-integrated PCB development kit: The BelayBox.**  
    It is available at https://shop.pionix.com with a touch screen display, 
    up to 22 kW 3ph AC charging, RCD, PCB-integrated power meter, RFID reader and 
    a Raspberry Pi CM4 compute module.
     
    Schematics and MCU firmware are open source:
    https://github.com/PionixPublic/reference-hardware

    Yocto for the BelayBox is available at:
    https://github.com/PionixPublic/dev-hardware-yocto

    Find the manual here: https://pionix.com/user-manual-belaybox
     
2. **AC DIN Rail / Dual public charging**  
    Dual socket AC charging with DIN rail contactors and power meters can be realized
    with the phyVERSO available from Phytec:  
    https://www.phytec.de/ladeelektronik/komplettloesung
     
    The advantage of this solution is the seamless transition to production: The phyVERSO
    is production-ready and can be used as is in volume production. Customization service
    is also available from Phytec to build custom derivatives with different interfaces
    and form factors to perfectly meet your requirements.  

3. **DC DIN Rail / Dual public charging**
    Similar to (2), the phyVERSO can be used in a DC configuration (both ports can be 
    configured for AC or DC; also a mixed configuration is possible). Phytec offers a
    DC development kit as well, which includes a 40 kW DC power supply, isolation monitor,
    power meter and everything else needed to make it a complete charger ready for
    evaluation.

.. note::

    Keep in mind that the development kits were not designed to be a certifiable product.
    They are optimized to be easily accessible for developers.

Start with your own hardware
----------------------------

If you already want to start integrating EVerest with
your existing charger hardware, we recommend to start reading through
the sections about setting up your Linux/Yocto and cross-compiling.
You will find these sections in the :doc:`Linux / Yocto overview </explanation/linux-yocto/index>`.

.. note::

   This is not the easiest way to start.
   But should you choose this mission, you will go the most direct way to use
   EVerest for production-ready charger development.
   Tell us about your experience and where you get stuck on the way.
   The mountain top can best be reached together.

We recommend to start your journey by copying an already existing image
(like the phyVERSO or Yeti/Yak ones) and change this according to your
needs and HW setup.

This will give you an overview on which in- and outputs are required,
the dependencies per module and how to set up the MQTT communication
accordingly.

-----------------------------------

**Authors**: Cornelius Claussen, Manuel Ziegler
