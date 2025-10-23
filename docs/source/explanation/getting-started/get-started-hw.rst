.. _exp_getting_started_hw:

############################
Getting Started with EVerest with Hardware
############################

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

1. **AC all-integrated PCB development kit: The Belaybox.**  
    It is available from https://shop.pionix.com with a touch screen display, 
    up to 22 kW 3ph AC charging, RCD, PCB-integrated power meter, RFID reader and 
    a Raspberry Pi CM4 compute module.
     
    Schematics and MCU firmware are open source:
    https://github.com/PionixPublic/reference-hardware

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

-----------------------------------

Authors: Cornelius Claussen, Manuel Ziegler