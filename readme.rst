========
 libfsm
========
--------------------------------------------------------------------
 a tiny c++14 library for writing maintainable finite state machines
--------------------------------------------------------------------

Features
========

* separation of **events**, **states** and **transitions**
* events support the transport of data
* asynchronous operation (threading support currently for **pthread** and **FreeRTOS**)
* heap free mode of operation



Overview
========

Declaring event types
---------------------

Use the helper class ``Identifiable`` to create types for each event::
    
    #include <fsm/utils/Identifiable.hpp>

    enum class EventID
    {
        Aquire,
        Release
    };

    using EventTypeFactory = fsm::utils::IdentifiableTypeFactory<EventID>;
    using EventBaseType = EventTypeFactory::Base;

    using EventAquire = EventTypeFactory::Derived<EventID::Aquire, int>;
    using EventRelease = EventTypeFactory::Derived<EventID::Release>;

    using EventBufferType = std::aligned_union_t<0, EventAquire>;


The enum type ``EventID`` is used to identify the different events later
on. ``IdentifiableTypeFactory`` is a helper type, which has the ``Base``
type definition corresponding to the base class type of all derived
event types.  This base class has an ``id`` property of type
``EventID``.  ``EventAquire`` for example is derived from
``EventBaseType`` with its ``id`` set to ``EventID::Aquire``. Derived
event types can be created using the ``Derived`` type definition of
``IdentifiableTypeFactory``.  ``Derived`` is templated and takes as its
first non-type template argument the specific ``EventID`` enum and as an
optional second template type argument the payload type of the event.
If this second argument is specified like for ``EventAquire``, then
instances of this event type have an additional member variable ``data``
of the specified type, that is::
    
    EventAquire ev {42};
    assert(ev.id == EventID::Aquire);
    assert(ev.data == 42);

The reason for this type structure is, that all derived events can be
submitted to the state machine (probably with attached payload) and
treated as a type ``EventBaseType&``, whose ``id`` member can then be
used to distinguish the different event types::

    void submit_event(EventBaseType& ev) {
        switch(ev.id) {
        case EventID::Aquire:
            // logic
            break;
        case EventID::Release:
            // logic
            break;
        }
    }

There is no usage of virtual functions here, in order to keep the event
types as small as possible - in fact, for event types without payload::

    sizeof(EventRelease) == sizeof(EventID)

Note, that the underlying type of a class enum can be specified to
further reduce the size::

    enum class EventID : uint8_t { ...

The advantage of this structure, in contrast to the plain enum type, is,
that these event types can carry payload data without any runtime
overhead.  For the design goal of having no heap allocations, a buffer
for an submitted event is necessary.  This buffer type can be deduced from
the largest event type, in this example::

    using EventBufferType = std::aligned_union_t<0, EventAquired>;

This has to be set manually, although it will be checked at compile
time, that this buffer is large enough to hold the submitted events.


Declaring the state id type
---------------------------

Each state handle has an id type, which can be used to identify the
state, for example::

    enum class State
    {
        Released,
        Locked
    };

    struct StateIDType {
        const State id;
        const std::string name;
    };

Here, again, an enum is used to distinguish different states.
Furthermore a string is part of the ``StateIDType`` for convenience.
The main reason for this is to attach any additional context or
information to the state handle, usable for all states.


Defining the state handles
--------------------------

The state machine controller handles different states by their
``StateHandleType``.  This is a template parameter and can be declared by::

    using EventInfoType = fsm::EventInfo<EventBaseType, EventBufferType>;
    using StateHandleType = fsm::StateHandle<EventInfoType, StateIDType>;

This type has three members

1. ``transitions``: this is a functor object representing the transition
   table for the current state. It takes two arguments.
   ``EventBaseType`` for deciding, where to go next and 
   ``TransitionWrapperType``, to setup the corresponding transition.

2. ``state_fun``: this is a functor object, representing the code which
   should be run for the current state.  It takes one argument of type
   ``FSMContextType``.  This type can be used by the state function, to
   wait for some time/condition and also to submit a new event.

3. ``id``: this a constant, carrying the state identification type

Finally, using the following type declarations, the state handles can be defined::

    using TransitionType = StateHandleType::TransitionWrapperType;
    using FSMContextType = StateHandleType::FSMContextType;

    StateHandleType released_state{{State::Released, "Released State"}};
    // now released_state.id == State::Released and released_state.name == "Released State"
    StateHandleType aquired_state{{State::Aquired, "Aquired State"}};

    // define transition table:
    released_state.transitions = [](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case EventID::Aquire:
            return trans.set(aquired_state);
        default:
            return;
        }
    };

    released_state.state_fun = [](FSMContextType& ctx) {
        // to some logic here, for example release after 1000ms
        if (ctx.wait_for(1000)) {
            // timeout
            ctx.submit_event(EventRelease());
        } else {
            // got cancelled due to new event or fsm stop
            // do something else
        }
    }

Both ``transisitions`` and ``state_fun`` are of type
``fsm::StaticThisFunctor``, which can be defined as a c++ lambda, that
can capture at least ``this`` - so that access to member
functions/variables is possible if the state handles are defined inside
an object.

The FSM controller
------------------

The fsm controller has the following api::

    // asynchronously spawn the main fsm loop with an initial state
    void run(StateHandleType& initial_state);

    // submitting an event, optionally blocking until it can be submitted
    // if it can't be submitted because there is already an ongoing transaction
    // or the state machine has been stopped, the function will return false
    bool submit_event(const typename StateHandleType::EventBaseType& event, bool blocking = false)

    // stop the main loop and wait for it to end
    void stop()

The basic fsm controller type has the following declaration::

    template <
        typename StateHandleType,
        typename PushPullLockType,
        typename ThreadType,
        typename ContextType,
        void (*LogFunction)(const std::string&) = nullptr
    > class BasicController

Different aspects of the controller are templated, so it remains portable:

1. ``StateHandleType``: see above

2. ``PushPullLockType``: system/platform dependent primitive,
   implementing a synchronized *push-pull* mechanism

3. ``ThreadType``: system/platform dependent primitive for spawning the
   run loop

4. ``ContexType``: system/platform dependent primitive,
   implementing the asynchronous communication between the main state
   function the state machine control Installation

5. ``LogFunction``: simple function reference for debugging

There exists two specializations of this basic controller for *posix
pthread*-like and *freertos* systems:

PThreadController
~~~~~~~~~~~~~~~~~

This is the reference fsm controller for *posix pthread* like systems.
Include ``<fsm/specialization/pthread.hpp>``, to use it.  Its
declaration is::

    template <typename StateHandleType, void (*LogFunction)(const std::string&) = nullptr>
        using PThreadController = ...

So you only need to define the ``StateHandleType``, and, optionally a
logging function.

FreeRTOSController
~~~~~~~~~~~~~~~~~~

This is the reference fsm controller for the *freertos* api.  Include
``<fsm/specialization/freertos.hpp>``, to use it.  Its declaration is::

    template <typename StateHandleType, int StackSize = 400, const char* Name = nullptr,
            void (*LogFunction)(const std::string&) = nullptr>
    using FreeRTOSController = ...

In addition to the ``PThreadController``, the ``StackSize`` for the
running loop, and the ``Name`` of the FreeRTOS *task* can be set.


Running the state machine
-------------------------

After:

1. declaring the event types

2. declaring the state id types

3. defining the state handles

4. choosing a controller implementation

the state machine, finally, can be used (with the above definitions and
the ``PThreadController``)::

    fsm::PThreadController<StateHandleType> fsm_ctrl;

    fsm_ctrl.run(released_state); // spawn run loop with initial state

    fsm_ctrl.submit_event(EventAquire({0})); // asynchronously send an event

    fsm_ctrl.stop(); // stop the state machine

Usage
=====

See the example in ``examples/light_switch``

Todo
====

- improve documentation

  - structure

  - how to install

- implement synchronous fsm controller type

- implement optimized ``StateHandleType`` for direct function-pointer
  only code
