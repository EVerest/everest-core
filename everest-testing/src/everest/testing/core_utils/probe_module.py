import asyncio
import logging
from queue import Queue
from typing import Any, Callable

from everest.framework import Module, RuntimeSession


class ProbeModule:
    """
    Probe module tool for integration testing, which is a thin abstraction over the C++ bindings from everestpy
    You need to declare the requirements for the probe module with the fixtures starting EVerest ("test_connections" argument),
    but you do not need to specify the interfaces provided by the probe - simply specify the implementation ID when registering cmd handlers and publishing vars.
    """

    def __init__(self, session: RuntimeSession, module_id="probe"):
        """
        Construct a probe module and connect it to EVerest.
        - session: runtime session information (path to EVerest installation and location of run config file)
        - module_id: the module ID to register with EVerest. By default, this will be "probe".
        """
        logging.info("ProbeModule init start")
        m = Module(module_id, session)
        self._setup = m.say_hello()
        self._mod = m
        self._ready_event = asyncio.Event()

        # subscribe to session events
        m.init_done(self._ready)
        logging.info("Probe module initialized")

    async def call_command(self, connection_id: str, command_name: str, args: dict) -> Any:
        """
        Call a command on another module.
        - connection_id: the id of the connection, as specified for the probe module in the runtime config
        - command_name: the name of the command to execute
        - args: the arguments for the command, as a name->value mapping
        returns: the result of the command invocation
        """

        interface = self._setup.connections[connection_id][0]
        try:
            return await asyncio.to_thread(lambda: self._mod.call_command(interface, command_name, args))
        except Exception as e:
            logging.info(f"Exception in calling {connection_id}.{command_name}: {type(e)}: {e}")

    def implement_command(self, implementation_id: str, command_name: str, handler: Callable[[dict], Any]):
        """
        Set up an implementation for a command.
        - implementation_id: the id of the implementation, as used by other modules requiring it in the runtime config
        - command_name: the name of the command to execute
        - handler: a function to handle the command, which takes a dict of arguments as input, and returns the return value as a dict (json object)
        Note: The handler runs in a separate thread!
        
        
        !!! WARNING: UNIMPLEMENTED COMMANDS MAY CAUSE EVEREST TO HANG !!!
        ----
        In the MQTT-based protocol used by EVerest, commands are initiated by publishing requests to a specific MQTT topic.
        The implementor is subscribed to this topic, and when they are done executing a command, they publish a result on the same topic. 
        To "implement" a command really just means to subscribe to the command's topic and attach a handler to process incoming requests there.
        If you do not implement a command, but another module tries to call it anyway, the command request won't reach anyone, and the caller will be stuck forever waiting for a response.
        If your tests hang, make sure you have implemented all commands which are called in the probe - the EVerest framework does not check this. 
        """
        self._mod.implement_command(implementation_id, command_name, handler)

    def publish_variable(self, implementation_id: str, variable_name: str, value: Any):
        """
        Publish a variable from an interface the probe module implements.
        - implementation_id: the id of the implementation, as used by other modules requiring it in the runtime config
        - variable_name: the name of the variable
        - value: the value to publish
        """
        self._mod.publish_variable(implementation_id, variable_name, value)

    def subscribe_variable(self, connection_id: str, variable_name: str, handler: Callable[[dict], None]):
        """
        Subscribe to a variable implemented by a module required by the probe module.
        - connection_id: the id of the connection, as specified for the probe module in the runtime config
        - variable_name: the name of the variable
        - handler: a function to handle incoming values for the variable, accepting a dict as an input, and returning nothing.
        Note: The handler runs in a separate thread!
        """
        self._mod.subscribe_variable(self._setup.connections[connection_id][0], variable_name, handler)

    def subscribe_variable_to_queue(self, connection_id: str, var_name: str):
        """
        The same as subscribe_variable, but incoming values will be pushed to a queue
        """
        queue = Queue()
        self._mod.subscribe_variable(self._setup.connections[connection_id][0], var_name,
                                     lambda message, _queue=queue: _queue.put(message))
        return queue

    def _ready(self):
        """
        Internal function: callback triggered by the EVerest framework when all modules have been initialized
        This is equivalent to the ready() method in C++ modules
        """
        logging.info("ProbeModule ready")
        self._ready_event.set()

    async def wait_to_be_ready(self, timeout=3):
        """
        Convenience method which allows you to wait until the _ready() callback is triggered (i.e. until EVerest is up and running)
        """
        await asyncio.wait_for(self._ready_event.wait(), timeout)
