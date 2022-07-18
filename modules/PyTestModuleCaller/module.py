from everestpy import log

setup_ = None

def pre_init(setup):
    global setup_
    setup_ = setup

def init():
    log.info(f"init")

def ready():
    log.info(f"ready")

    setup_.r_yeti_extras.call_firmware_update(firmware_binary="HeLlO")
    log.error(f"woop")

    setup_.r_evse_manager.call_enable()
    setup_.r_evse_manager.call_disable()
    setup_.r_evse_manager.call_enable()
    log.error(f"woop")
