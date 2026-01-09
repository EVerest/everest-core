.. error_framework:

###############
Error Framework
###############

This explains the general idea and the components provided by the error framework.
For practical hints on the usage of the error framework, consult the
:ref:`error framework how-to guide <htg_error_framework>`.

*******
Purpose
*******

The error framework is used to handle errors in the EVerest framework.

As not every module can "decide" by itself how to react to an error and how
to handle it, the error framework provides functionality that allows modules
to react to errors that are raised in required other modules.

The other main purpose of the error framework is to provide a way to monitor
and report errors in the system. This can be used for example for displaying
or reporting to an OCPP backend.

*****
Usage
*****

General
=======

The Error class
---------------

The Error class is a simple struct that holds all required information
required to handle the error. Data members include:

- type
- sub-type
- arbitrary message + description
- the raising module's ID
- vendor_id
- severity
- timestamp
- uuid
- state (active, cleared, ...)

Raise an error
--------------

Each implementation of an interface can raise errors that are defined in the
interface. There is one function `raise_error` that takes an error object as
argument. The error object is an instance of the class `Error`. To create
the initial error object, the `ErrorFactory` is used.

Clear an error
--------------

An error can be cleared by the same implementation that raised the error. There
are multiple functions to clear an error.

The function `clear_error` provides two different overloads. The first
overload takes the `ErrorType` as argument and clears errors with matching
`ErrorType`.

The second overload takes `ErrorType` and `ErrorSubType` as arguments.
In this case, only the error with matching `ErrorType` and `ErrorSubType`
is cleared.

The function `clear_all_errors_of_impl` clears all errors of the current
implementation.

Subscribe to an error
---------------------

A module can subscribe to errors of its requirements. This way the module can
react to those errors. There are two functions to subscribe to errors.

The function `subscribe_error` takes the `ErrorType` and two callback functions
as arguments.
The `ErrorType` is the type of the error that the module wants to subscribe to.
The first callback is called when the error is raised.
The second callback is called when the error is cleared.

The function `subscribe_all_errors` takes again two callback functions as
arguments. The first callback is called when an error of any type is raised by
the requirement. The second callback is called when an error is cleared.

Subscribe globally to all errors
--------------------------------

This feature is currently only available for C++ modules. It allows a module
to subscribe globally to all errors of all modules. This can be used for
example for logging purposes or error reporting.

To enable this functionality, the flag `enable_global_errors` in the module's
manifest file must be set to `true`.
With this, the function `subscribe_global_all_errors` is added to the
auto-generated code. This way the function can be used as the other subscribe
functions, with two callback functions as arguments.

The ErrorFactory
----------------

Since a module does not have direct access to some information that is required
to create an error object, as for example the `module_id`, the class
`ErrorFactory` is used, which is provided for each implementation of an
interface, with correct default values.

The `ErrorFactory` is used to create the initial error object.
This error object can be raised as it is, or can be modified before raising.

The ErrorFactory provides five signatures for the create_error function:

- *Default*: Takes no arguments and initializes with default values.
- *Standard*: Requires `ErrorType`, `ErrorSubType`, and `message`.
- *Contextual*: Extends the *standard* signature by adding either `severity`, `state`, or *both*.

The ErrorStateMonitor
---------------------

The `ErrorStateMonitor` is a class that is used to monitor the error state of
implementations and requirements.
It is provided for each implementation of an interface and for each requirement
of an module.

To check if an error is currently active, the function `is_error_active` is
used. This function takes the `ErrorType` and `ErrorSubType` as arguments and
returns a boolean value. If the error is active, the function returns `True`,
otherwise `False`.

To check if a specific set of errors is in a specific state, the struct
`StateCondition` is defined.
This struct has the members `ErrorType`, `ErrorSubType` and `active: boolean`.
The function `is_condition_satisfied` can either take a single `StateCondition`
or a list of `StateCondition` as argument.
If a single `StateCondition` is passed, the function returns `True` if the
error is in the state as defined in the `StateCondition`.
If a list of `StateCondition` is passed, the function returns `True` if all
conditions are satisfied.

***********
Usage Guide
***********

Creating Error objects
======================

Error objects may always be created using the `ErrorFactory` of the
implementation.

Error objects can be edited after creation, before raising them.

The following attributes may not be changed after creation:

- `timestamp`
- `origin.module_id`
- `origin.implementation_id`
- `uuid`

The global subscription
=======================

If a module is subscribed to global all errors, it may do only "reporting"
actions, but no "handling" actions. This means that the module does not change
its behavior based on the error, but only reports the error for example to a
log file.

Side effects of raising errors
==============================

The error framework allow module implementations to get notified about an error 
from one of their requirements by subscribing to the error. This can be used for 
reporting purposes (e.g. via OCPP) or it can be used to adjust the control flow
of the module based on the raised error.

It is important to note that raising errors can therefore lead to side effects
within other modules. The side effects shall be documented as part of the module
documentation (see e.g. EvseManager or OCPP).
