.. _how_to_renesas_mpu:

##############################################
How to for Renesas MPU (RZ/G2L family)
##############################################

To get more information on EVerest and Renesas hardware, see here:
https://www.renesas.com/en/products/microcontrollers-microprocessors/rz-mpus/rz-partner-solutions/pionix-basecamp

Here is how to set it up and run an EVerest simulation:

1. Clone the ``rz-community-bsp`` repo:
   https://github.com/renesas-rz/rz-community-bsp.git

2. Apply the patch necessary to build ``everest-core``.
   You can find the patch here: :ref:`Patch file <how_to_renesas_mpu_patch>`

3. Start the ``kas-container`` menu to configure the environment:

   a. Run ``./kas-container menu``
   b. Select the device **RZ/G2L**
   c. Save & Exit

4. Start ``kas-shell`` with ``./kas-container shell`` and install dependencies:

   a. Run ``sudo apt-get update``  
   b. Run ``sudo apt-get install -y python3.11 python3.11-dev``

5. Edit the file  
   ``/work/poky/meta/recipes-devtools/elfutils/elfutils_0.186.bb``  
   to add the following line:

   .. code-block:: bash

      CFLAGS:append = " -Wno-error=deprecated-declarations"

6. Start the build using:

   ``bitbake renesas-image-minimal``

   Once the build is complete, exit the shell.

7. Flash the hardware with the instructions in the RZ/G2L startup guide:
   `Evaluation Board Kit Quick Start Guide <https://www.renesas.com/us/en/document/qsg/rzg2l-evaluation-board-kit-quick-start-guide>`_

   The generated images/binaries will be present in:

   .. code-block:: bash

      built/tmp/deploy/images/smarc-rzg2l

8. Start EVerest with:

   .. code-block:: bash

      /usr/bin/manager --conf /etc/everest/config-sil.yaml

If everything has been set up correctly, you will now be able to run simulation
steps with EVerest. See the :ref:`Quick Start Guide <htg_getting_started_sw>`
for more information.

To go further and implement your own customized EVerest modules, have a look at
the :doc:`EVerest module concept documentation </explanation/detail-module-concept>`.

----

**Authors:** Manuel Ziegler
