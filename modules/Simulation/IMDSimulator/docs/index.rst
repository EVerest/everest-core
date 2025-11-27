.. _everest_modules_handwritten_IMDSimulator:

############
IMDSimulator
############

External MQTT Control
=====================

The IMDSimulator module supports setting the simulated isolation resistance via MQTT.
To set the resistance, publish an integer value (in Ohms) to the following topic:

.. code-block:: none

    everest_api/<module_id>/cmd/set_resistance
