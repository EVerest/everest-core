.. _htg_error_framework:

#########################
Using the Error Framework
#########################

Syntax in a C++ module
======================

You can find two example modules written in C++ in the `examples` folder:
`ExampleErrorRaiser` and `ExampleErrorSubscriber`.

Raise an error
--------------

Can be done in the implementation of an interface.

.. code-block:: cpp

    // Create an error object
    Error error_object = this->error_factory->create_error(
        "example/ExampleErrorA",    // ErrorType
        "",                         // ErrorSubType
        "This is an example error"  // message
    );
    // Raise the error
    raise_error(error_object);


Clear an error
--------------

Can be done in the implementation of an interface.

.. code-block:: cpp

    // Clear all errors of the ErrorType "example/ExampleErrorA"
    clear_error(
        "example/ExampleErrorA",    // ErrorType
        true                        // clear_all
    );

    // Clear the error with ErrorType "example/ExampleErrorA" and ErrorSubType ""
    clear_error(
        "example/ExampleErrorA",    // ErrorType
        ""                          // ErrorSubType
    );
    clear_error(
        "example/ExampleErrorA",    // ErrorType
        false                       // clear_all
    );
    clear_error(
        "example/ExampleErrorA"     // ErrorType
    );                              // clear_all defaults to false

    // Clear all errors of the current implementation
    clear_all_errors_of_impl();

Subscribe to an error
---------------------

May be done in the `init` function of the implementation.

.. code-block:: cpp

    // Subscribe to an error of the ErrorType "example/ExampleErrorA"
    subscribe_error(
        "example/ExampleErrorA",                    // ErrorType
        [](Error error) {                           // callback
            // Do something when the error is raised
        },
        [](Error error) {                           // clear_callback
            // Do something when the error is cleared
        }
    );

    // Subscribe to all errors of the requirement
    subscribe_all_errors(
        [](Error error) {                           // callback
            // Do something when an error is raised
        },
        [](Error error) {                           // clear_callback
            // Do something when an error is cleared
        }
    );

Subscribe to global all errors
------------------------------

Needs to be enabled in the manifest file of the module. May be done in the
`init` function of the implementation.

.. code-block:: cpp

    // Subscribe to global all errors
    subscribe_global_all_errors(
        [](Error error) {                           // callback
            // Do something when an error is raised
        },
        [](Error error) {                           // clear_callback
            // Do something when an error is cleared
        }
    );

The ErrorFactory
----------------

Is used to create an error object.

.. code-block:: cpp

    Error error_object_0 = this->error_factory->create_error();

    Error error_object_1 = this->error_factory->create_error(
        "example/ExampleErrorA",        // ErrorType
        "",                             // ErrorSubType
        "This is an example error"      // message
    );

    Error error_object_2 = this->error_factory->create_error(
        "example/ExampleErrorA",        // ErrorType
        "",                             // ErrorSubType
        "This is an example error",     // message
        Everest::error::Severity::High  // severity
    );

    Error error_object_3 = this->error_factory->create_error(
        "example/ExampleErrorA",        // ErrorType
        "",                             // ErrorSubType
        "This is an example error",     // message
        Everest::error::State::Active   // state
    );

    Error error_object_4 = this->error_factory->create_error(
        "example/ExampleErrorA",        // ErrorType
        "",                             // ErrorSubType
        "This is an example error",     // message
        Everest::error::Severity::High, // severity
        Everest::error::State::Active   // state
    );

The ErrorStateMonitor
---------------------

Is used to monitor the error state of implementations and requirements.
Can be accessed in the implementation of an interface / anytime for
requirements.

Get the `ErrorStateMonitor`:

.. code-block:: cpp

    // Get the ErrorStateMonitor of an implementation
    std::shared_ptr<ErrorStateMonitor>& monitor = this->error_state_monitor;

    // Get the ErrorStateMonitor of a requirement
    std::shared_ptr<ErrorStateMonitor>& monitor = this->mod->r_example_raiser->error_state_monitor;

Check if an error is active:

.. code-block:: cpp

    // Check if an error of the ErrorType "example/ExampleErrorA" is active
    bool is_active = monitor->is_error_active(
        "example/ExampleErrorA",    // ErrorType
        ""                          // ErrorSubType
    );

Check if a specific set of errors is in a specific state:

.. code-block:: cpp

    // Check if an error of the ErrorType "example/ExampleErrorA" is active
    StateCondition condition = {
        "example/ExampleErrorA",        // ErrorType
        "",                             // ErrorSubType
        true                            // active
    };
    bool is_satisfied = monitor->is_condition_satisfied(condition);

    // Check if multiple errors are active
    std::list<StateCondition> conditions = {
        {
            "example/ExampleErrorA",    // ErrorType
            "",                         // ErrorSubType
            true                        // active
        },
        {
            "example/ExampleErrorB",    // ErrorType
            "",                         // ErrorSubType
            true                        // active
        }
    };
    bool are_satisfied = monitor->is_condition_satisfied(conditions);

Syntax in a Python module
=========================

You can find two example modules written in Python in the `examples` folder:
`PyExampleErrorRaiser` and `PyExampleErrorSubscriber`.

The error related classes need to be imported from the `everest` module.

.. code-block:: python

    from everest.framework import error

Raise an error
--------------

Can be done in the implementation of an interface after initializing.
In opposite to the C++ implementation, the raise function is called on the
module object and takes additionally the `implementation_id` as argument.

.. code-block:: python

    # Create an error object
    error_object = self._mod.get_error_factory("example_raiser").create_error(
        "example/ExampleErrorA",    # ErrorType
        "",                         # ErrorSubType
        "This is an example error"  # message
    )
    # Raise the error
    self._mod.raise_error(
        "example_raiser",           # implementation_id
        error_object                # error
    )

Clear an error
--------------

Can be done in the implementation of an interface after raising.
In opposite to the C++ implementation, the clear function is called on the
module object and takes additionally the `implementation_id` as argument.

.. code-block:: python

    # Clear all errors of the ErrorType "example/ExampleErrorA"
    self._mod.clear_error(
        "example_raiser",           # implementation_id
        "example/ExampleErrorA",    # ErrorType
        True                        # clear_all
    )

    # Clear the error with ErrorType "example/ExampleErrorA" and ErrorSubType ""
    self._mod.clear_error(
        "example_raiser",           # implementation_id
        "example/ExampleErrorA",    # ErrorType
        ""                          # ErrorSubType
    )
    self._mod.clear_error(
        "example_raiser",           # implementation_id
        "example/ExampleErrorA",    # ErrorType
        False                       # clear_all
    )
    self._mod.clear_error(
        "example_raiser",           # implementation_id
        "example/ExampleErrorA"     # ErrorType
    )                               # clear_all defaults to false

    # Clear all errors of the current implementation
    self._mod.clear_all_errors_of_impl(
        "example_raiser"            # implementation_id
    )

Subscribe to an error
---------------------

Can be done in the `init` function of the implementation.
In opposite to the C++ implementation, the subscribe function is called on the
module object and takes additionally the `requirement` as argument.

.. code-block:: python

    # Subscribe to an error of the ErrorType "example/ExampleErrorA"
    self._mod.subscribe_error(
        self._setup.connections["example_raiser"][0],   # requirement
        "example/ExampleErrorA",                        # ErrorType
        lambda error: print("Error raised: ", error),   # callback
        lambda error: print("Error cleared: ", error)   # clear_callback
    )

    # Subscribe to all errors of the requirement
    self._mod.subscribe_all_errors(
        self._setup.connections["example_raiser"][0],   # implementation_id
        lambda error: print("Error raised: ", error),   # callback
        lambda error: print("Error cleared: ", error)   # clear_callback
    )

Subscribe to global all errors
------------------------------

This feature is currently only available for C++ modules.

The ErrorFactory
----------------

Is used to create an error object.

.. code-block:: python

    error_object_0 = self._mod.get_error_factory("example_raiser").create_error()

    error_object_1 = self._mod.get_error_factory("example_raiser").create_error(
        "example/ExampleErrorA",        # ErrorType
        "",                             # ErrorSubType
        "This is an example error"      # message
    )

    error_object_2 = self._mod.get_error_factory("example_raiser").create_error(
        "example/ExampleErrorA",        # ErrorType
        "",                             # ErrorSubType
        "This is an example error",     # message
        error.Severity.High             # severity
    )

    error_object_3 = self._mod.get_error_factory("example_raiser").create_error(
        "example/ExampleErrorA",        # ErrorType
        "",                             # ErrorSubType
        "This is an example error",     # message
        error.State.Active              # state
    )

    error_object_4 = self._mod.get_error_factory("example_raiser").create_error(
        "example/ExampleErrorA",        # ErrorType
        "",                             # ErrorSubType
        "This is an example error",     # message
        error.Severity.High,            # severity
        error.State.Active              # state
    )

The ErrorStateMonitor
---------------------

Get the `ErrorStateMonitor`:

.. code-block:: python

    # Get the ErrorStateMonitor of an implementation
    monitor = self._mod.get_error_state_monitor_impl(
        "example_raiser"                                # implementation_id
    )

    # Get the ErrorStateMonitor of a requirement
    monitor = self._mod.get_error_state_monitor_req(
        self._setup.connections["example_raiser"][0]    # requirement
    )

Check if an error is active:

.. code-block:: python

    # Check if an error of the ErrorType "example/ExampleErrorA" is active
    is_active = monitor.is_error_active(
        "example/ExampleErrorA",    # ErrorType
        ""                          # ErrorSubType
    )

Check if a specific set of errors is in a specific state:

.. code-block:: python

    # Check if an error of the ErrorType "example/ExampleErrorA" is active
    condition = error.StateCondition(
        "example/ExampleErrorA",    # ErrorType
        "",                         # ErrorSubType
        True                        # active
    )
    is_satisfied = monitor.is_condition_satisfied(condition)

    # Check if multiple errors are active
    conditions = [
        error.StateCondition(
            "example/ExampleErrorA",    # ErrorType
            "",                         # ErrorSubType
            True                        # active
        ),
        error.StateCondition(
            "example/ExampleErrorB",    # ErrorType
            "",                         # ErrorSubType
            True                        # active
        )
    ]
    are_satisfied = monitor.is_condition_satisfied(conditions)
