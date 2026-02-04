.. _everest_modules_handwritten_PySunspecServer:

.. *******************************************
.. PySunspecServer
.. *******************************************

Skeleton for a PySunspecServer module. This module receives powermeter measurements and can set external limits for an EVSE.
This module currently only serves as an example and misses the actual implementation of a Sunspec server.

After building everest-core, you can run this module with a suitable configuration
(`config-sil-sunspec.yaml <../../../config/config-sil-sunspec.yaml>`_) in the SIL.

.. code-block:: bash

    ./run-scripts/run-sil-sunspec.sh
