.. _exp_linux_yocto_building_yocto:

##############################
Building a Yocto for your custom hardware
##############################

When creating your custom Yocto for your target hardware, the best is to
start with the Yocto supplied by the SoM/Board/CPU manufacturer instead
of using the one for the BelayBox.

Many SoM manufacturers provide quite well-maintained Yocto
distributions, e.g. PHYTEC provides an *ampliPHY* distribution for all
of their SoMs. That already solves a lot of things. We will look at that
in the next chapters.

.. tip::

   The SoM manufacturer may also already provide suitable build containers or
   other tools to help with integration into your CI/CD.

After the basic setup is done, you most likely will need to adapt the
device tree to your custom board to mux all pins correctly and map Linux
kernel drivers to the correct peripherals.

Once it boots correctly and all hardware is initialized in the kernel,
you can start with adding the *meta-everest* Yocto layer to your
bblayers.conf. You may also want to create your own layer for your image
files, EVerest config files, recipes for your own software etc. Look at
*meta-everest* as an example and refer to the Yocto documentation:

https://docs.yoctoproject.org/dev/dev-manual/layers.html
https://github.com/EVerest/meta-everest

Then - in case you have not done it yet -, create a custom image file
for your board that installs EVerest as well as all other tools you may
want to have on your base system. This image can be based e.g. on
*core-image-minimal*. Here is an example:

TODO: verify yocto build for belaybox!

.. code-block:: bitbake

   require recipes-core/images/core-image-minimal.bb

   SUMMARY = "EVerest image for PIONIX BelayBox development kit"

   LICENSE = "MIT"

   CORE_IMAGE_EXTRA_INSTALL += "\
           everest-core \
           libocpp \
           openssh \
           mosquitto \
           tzdata \
           pionixbox \
           flutter-pi \
           flutter-engine \
           fontconfig \
           ttf-roboto \
           htop \
           tmux \
       "

The minimal required packages are ``everest-core`` and ``mosquitto``. The
package ``tmux`` is only needed for the BringUp & Qualification tools.
Other debugging tools that may be useful during development phase are:

.. code-block:: bitbake

   tcpdump
   canutils
   tpm-tools
   open-plc-utils
   ethtool

.. note::

   The locale should be set to UTF-8 (otherwise BringUp & Qualification tools
   will look weird).

--------------------------------

Authors: Cornelius Claussen, Manuel Ziegler
