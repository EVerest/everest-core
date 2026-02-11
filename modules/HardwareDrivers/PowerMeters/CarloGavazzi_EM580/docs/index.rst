.. _everest_modules_handwritten_CarloGavazzi_EM580:

.. *******************
.. Carlo Gavazzi EM580
.. *******************

Module implementing the Carlo Gavazzi EM580 power meter driver adapter via Modbus RTU (through SerialCommHub).

Description
===========

The module consists of a single ``main`` implementation that serves the ``powermeter`` interface. Modbus access is done
via the required ``serial_communication_hub`` interface.

Features
========

- Live meter reads and ``powermeter`` publishing (interval configurable)
- OCMF/Eichrecht transaction start/stop logic
- Public key reading and publishing (hex)
- Resilient Modbus transport with retries and protocol-compliant write chunking

Module Configuration
====================

The module configuration parameters are defined in ``manifest.yaml``. A complete example configuration can be found at
``config/bringup/config-bringup-CGEM580.yaml``.

Transaction flow (OCMF)
=======================

Start transaction
-----------------

At transaction start the module:

1. Ensures OCMF state is ``NOT_READY``
2. Writes OCMF identification data, EVSE ID and tariff text (TT) (0-terminated, used bytes only)
3. Writes session modality
4. Sends the start command (``'B'``)

Stop transaction
----------------

At transaction stop the module:

1. Sends the end command (``'E'``) for the tracked transaction
2. Waits for OCMF state ``READY``
3. Reads the OCMF file (size + content)
4. Confirms the file read by setting state back to ``NOT_READY``

Notes / Limitations
===================

- Modbus ``Write Multiple Registers`` requests are chunked to max 123 registers per request.
- TT is a ``CHAR[252]`` field (126 words); overlong strings are warned and truncated.


