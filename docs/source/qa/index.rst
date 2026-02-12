#####
Q & A
#####

This page will grow with questions from the mailing list and topics that
come up regularly in our EVerest development life. It is always a good idea
to have a look here when running into problems before asking for help via
the :ref:`mailing list <exp_community_mailinglist>`.

Errors, warnings and Troubleshooting
====================================

Compiling with GNU compilers
----------------------------

Building EVerest, you might want to use a GNU compiler. Handing over the flag
`CMAKE_CXX_COMPILER` to `cmake` lets you do that.

However, when using `gcc`, you might get errors about some
`unreferenced symbols` or linking issues.

Solution is simple: Use `g++` instead::

  cmake -D CMAKE_CXX_COMPILER=g++

`g++` will link std C++ files automatically
(`besides others <https://stackoverflow.com/a/173007/1168315>`_) which `gcc` won't do.

RPC communication timeout
-------------------------

**In the Admin Panel, I sometimes get the following error when saving a config
file:**

.. code-block::

  Failed to save test_config Reason: RPC communication timeout to everest
  controller process.

**How can I solve this?**

In this case, the Admin Panel timeouted while waiting for the response of the
EVerest process trying to save the file.

The timeout is currently 2s.

The problem with your setup might be that running EVerest as well as running
an UI session with a browser on one hardware is just too much for it. This
can sometimes happen on Raspberry Pies, for example.

You may try to connect from a desktop PC to IP_OF_THE_RASPBERRY:8849. This way,
the client-side processing of the Admin Panel javascript code gets offloaded
from the Raspberry and it might be able to process the save faster.

Another hint for environments with very limited ressources is to fill in the
workspace information into the yaml config manually without using the Admin
Panel.

EVerest OCPP 2.0.1 setup
------------------------
After successfully setting up EVerest and configuring the
:doc:`OCPP201 module <everest_modules_OCPP201:>`, I get errors about
a failed websocket connection.

The :doc:`OCPP module <everest_modules_OCPP201:>` of EVerest operates - for now - as an OCPP client.
You will need to choose a backend system capable of OCPP 2.0.1 (like SteVe
for OCPP 1.6).

You may want to have a look at `<https://github.com/mobilityhouse/ocpp>`_ and
implement message handlers to get the communication working. Or you can have
a look at `<https://github.com/thoughtworks/maeve-csms>`_. Note: This has not been
officially tested by us.
