from everestpy import log
import asyncio
import logging
import json


async def main():
    """
    Entrypoint function
    """
    logging.info("test_control_module up and running")

    # you can use this to request a shutdown of all everest modules
    # setup_.request_shutdown()


def run():
    """Function to run the PyTestControlModule thread
    """
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logging.error("test_control_module terminated manually")


setup_ = None
module_config_ = None
impl_configs_ = None
module_info_ = None
car_sim_handle_ = None
evse_handles_ = []
charging_done_callback_function = None
event_callback_function = None


def pre_init(setup, module_config, impl_configs, _module_info):
    """Default pre-init function for PyTestControlModule

    Args:
        setup (Any): everestpy setup data
        module_config (Any): everestpy module_config data
        impl_configs (Any): everestpy impl_config data
        _module_info (Any): everestpy module_info data
    """
    global setup_, module_config_, impl_configs_
    setup_ = setup
    module_config_ = module_config
    module_info_ = _module_info
    impl_configs_ = impl_configs


def init():
    """This function initializes the PyTestControlModule.

       Sets up the EVSE and CarSimulator connections.
    """
    global car_sim_handle_
    global evse_handles_

    logging.info("test_control_module init")

    car_sim_handle_ = setup_.r_test_control

    for req_id_module, connector_1 in setup_.r_connector_1.items():
        evse_handles_.append([req_id_module, connector_1])
        connector_1.subscribe_session_event(process_session_event)

    if len(setup_.r_connector_1.items()) < 1:
        logging.debug("No EVSE connector available.")


def ready():
    """Default pre-init function for PyTestControlModule.

       Starts the module's main thread.
    """
    logging.info("PyTestControlModule ready")
    run()

def shutdown():
    log.info("Shutting down PyTestControlModule module...")


def set_event_callback(callback_func):
    """This function has to be called from the everest-testing framework at the startup of a test.

       It sets up a callback function to push events from the everest-core execution (e.g. EVSE events)
       to the corresponding everest-testing validation function.

    Args:
        callback_func (function handle): Pushes event data to everest-testing.
    """
    global event_callback_function
    event_callback_function = callback_func


# CarSimulator controls
def start_charging(mode='raw', charge_sim_string=''):
    """This function prepares the CarSimulator and enables its charging request.

    Args:
        mode (str, optional): Sets the charging mode in the CarSimulator. Defaults to 'raw'.
        charge_sim_string (str): Behavior description for the charging process inside the CarSimulator.
    """
    logging.info('start_charging() called')

    if charge_sim_string == '':
        charge_sim_string = "sleep 1;iec_wait_pwr_ready;sleep 1;draw_power_regulated 16,3;sleep 25;unplug"

    car_sim_enable_request = json.loads('true')
    car_sim_handle_.call_enable(car_sim_enable_request)

    car_sim_handle_.call_executeChargingSession(charge_sim_string)


# process EVSE subscriptions
def process_session_event(data):
    """This function pushes incoming event data (from an EVSE manager inside the everest-core instance)
       to the corresponding everest-testing validation function.

       Needs to be set up with 'set_event_callback()' during test init in the everest-testing framework.

    Args:
        data (Any): Event data emitted by the EVSE manager in everest-core.
    """
    if event_callback_function is not None:
        try:
            event_callback_function(data)
        except NameError:
            logging.debug(f"could not call event_callback_function(); data: {data}")
    else:
        logging.debug(f"WARNING: event_callback_function is not set! data: {data}")


# EVSE commands
def pause_charging(evse_number=0):
    """This function sends a 'pause_charging()' command to the selected EVSE manager in everest-core.

    Args:
        evse_number (int, optional): Which EVSE manager to send this command to. Defaults to 0.
    """
    if evse_handles_  != []:
        evse_handles_[evse_number].call_pause_charging()
    else:
        logging.debug("Attempt to call 'pause_charging()', but no EVSE connection available!")


def resume_charging(evse_number=0):
    """This function sends a 'resume_charging()' command to the selected EVSE manager in everest-core.

    Args:
        evse_number (int, optional): Which EVSE manager to send this command to. Defaults to 0.
    """
    if evse_handles_  != []:
        evse_handles_[evse_number].call_resume_charging()
    else:
        logging.debug("Attempt to call 'resume_charging()', but no EVSE connection available!")


def set_local_max_current(evse_number=0, max_current_A=0.0):
    """This function sends a 'set_local_max_current()' command to the selected EVSE manager in everest-core.

    Args:
        evse_number (int, optional): Which EVSE manager to send this command to. Defaults to 0.
        max_current_A (float): Which maximum current to set in the EVSE manager in Ampere. Defaults to 0.0 A.
    """
    if evse_handles_  != []:
        evse_handles_[evse_number].call_set_local_max_current(max_current_A)
    else:
        logging.debug("Attempt to call 'set_local_max_current()', but no EVSE connection available!")
