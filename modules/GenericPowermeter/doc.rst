.. _everest_modules_handwritten_GenericPowermeter:

************************
GenericPowermeter
************************

See also module's :ref:`auto-generated reference <everest_modules_GenericPowermeter>`.

The module ``GenericPowermeter`` provides a connection to get data from 
powermeters that are connected via Modbus RTU and whose data is automatically 
updated (without startup configuration and/or request functions). 

It supports both AC and DC powermeters.

To do so, a register configuration file is needed that describes which (ModbusRTU-)registers
are available on the device and what data they contain. The available data will then be used
to calculate the correct structure of data for the ``powermeter`` interface of everest-core.

Currently available powermeter configurations can be found in the module's ``models`` 
subdirectory.

To add a new powermeter to the set of available devices, there exists a ``template.yaml``
file in the ``models`` subdirectory which can be copied and filled with the new powermeter's
data from the datasheet.


Datasets in the configuration file
==================================

Available datasets for the module ``GenericPowermeter``
-------------------------------------------------------

This module can read the following parameters from a powermeter:

(for a description of the parameter values see interface ``powermeter``)

* energy_Wh_import
* energy_Wh_export
* power_W
* voltage_V
* VAR
* current_A
* frequency_Hz

Dataset description
-------------------

A typical dataset consists of the following parameters:

(<start_register> of length <num_registers>) * <multiplier> * 10^(<exponent_register>)

* <start_register> = the device's ModbusRTU register at which the value for the data of this 
  type is being stored (set to "0" if this value is not available in the powermeter)
* <function_code_start_reg> = ModbusRTU function code used to obtain this register's value
  (currently implemented: ``3`` (``read holding registers``) and ``4`` (``read input registers``))
* <num_registers> = the number of registers to read from the address of <start_register>
* <multiplier> = a multiplier to manually (pre-)scale the register's value (i.e. set to ``0.001`` 
  when the powermeter provides energy in ``kWh``, as the ``powermeter`` interface uses energy 
  in ``Wh``)
* <exponent_register> = the device's ModbusRTU register at which the exponent for the 
  register set is being stored (set to "0" if this value is not available in the powermeter)
* <function_code_exp_reg> = ModbusRTU function code used to obtain this register's exponent 
  (currently implemented: ``3`` (``read holding registers``) and ``4`` (``read input registers``))


Structure of datasets in the configuration file
-----------------------------------------------

Datasets are structured into two levels:

* first level : contains either DC value or, in AC case, sum total of all corresponding lines (L1/2/3)
* second level : contains AC values, split into their corresponding lines (L1/2/3)

Other things to note
--------------------

* if measuring AC, the first level of registers is always "total/sum" of a certain value and 
  the L1/2/3 registers are for the distinct phases
* if measuring DC, only use the first level of registers

Published variables
===================

powermeter
----------

The basic dataset of powermeter values as used in the EVerest ``powermeter`` interface.
This dataset will be periodically published by the module.


Provided commands
=================

get_signed_meter_value
----------------------

`currently not implemented`