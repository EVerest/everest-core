.. _how_to_renesas_mpu:

##################################
How to for Renesas MPU (RZ/G2L family)
##################################

To get more information on EVerest and Renesas hardware, see here:
https://www.renesas.com/en/products/microcontrollers-microprocessors/rz-mpus/rz-partner-solutions/pionix-basecamp

Here is how to set it up and run an EVerest simulation:

1. Clone the rz-community-bsp repo: https://github.com/renesas-rz/rz-community-bsp.git

2. Apply the patch, which you can find on the following page. The changes are
  necessary to build everest-core:
  :ref:`Patch file <how_to_renesas_mpu_patch>`

3. Start kas-container menu to configure the environment:
  (a) Run ./kas-container menu
  (b) Select device as RZ/G2L.
  (c) Save & Exit

4. Start kas-shell with ./kas-container shell:
  (a) Run *sudo apt-get update*
  (b) Run *sudo apt-get install -y python3.11 python3.11-dev*

5. Edit the file */work/poky/meta/recipes-devtools/elfutils/elfutils_0.186.bb*
  to add the following line:

  .. code-block:: bash

    CFLAGS:append = " -Wno-error=deprecated-declarations"

6. Finally, start the build: *bitbake renesas-image-minimal*
  Once the build is complete, exit the shell.

7. Flash the hardware with the information in the RZ/G2L startup guide, which
  can be found here:
  `Evaluation Board Kit Quick Start Guide <https://www.renesas.com/us/en/document/qsg/rzg2l-evaluation-board-kit-quick-start-guide>`_

  The generated image/binaries will be present in the directory:

  .. code-block:: bash

    built/tmp/deploy/images/smarc-rzg2l

7. Start EVerest with the following command:

  .. code-block:: bash

    /usr/bin/manager --conf /etc/everest/config-sil.yaml

If everything has been set up correctly, you will be able to run simulation
steps with EVerest now. Have a look at the
:ref:`Quick Start Guide <quickstartguide_main>` to get more information on
this.

From there on, you might want to take it to the next level by implementing
your own customized EVerest modules. Have a look at the dedicated
:ref:`EVerest module concept documentation <moduleconcept_main>` to get an
idea of doing that.

---------------------------------------------

Authors: Manuel Ziegler