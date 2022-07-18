from everestpy import log

setup_ = None

def pre_init(setup, _mod_conf, _impl_conf):
    global setup_
    setup_ = setup

def init():
    log.info(f"init")

def ready():
    log.info(f"ready")

    for name, yeti_extra in setup_.r_yeti_extras.items():
        yeti_extra.call_firmware_update(firmware_binary="HeLlO")

    log.error(f"woop")

    for name, evse_manager in setup_.r_evse_manager.items():
        evse_manager.call_enable()
        evse_manager.call_disable()
        evse_manager.call_enable()
    log.error(f"woop")
