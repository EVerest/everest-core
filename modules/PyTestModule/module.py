from everestpy import log

setup_ = None
module_config_ = None
impl_configs_ = None

def yeti_extras_firmware_update(firmware_binary: str):
    log.error(f"FIRMWARE UPDATE: {firmware_binary}")

def evse_manager_enable():
    log.error("ENABLE")
    return True

def evse_manager_disable():
    log.error("DISABLE")
    return True

def limits_callback(limits):
    log.critical(f"Received limits: {limits}")


def pre_init(setup, module_config, impl_configs):
    global setup_, module_config_, impl_configs_
    setup_ = setup
    module_config_ = module_config
    impl_configs_ = impl_configs


def init():
    log.info(f"init")


def limits(data):
    log.error(f"limits: {data}")

def ready():
    log.info(f"ready")

    log.info(f"Module config: {module_config_}")
    log.info(f"Impl configs: {impl_configs_}")

    log.info("Setup available")
    setup_.mqtt.publish("test2", "hello2")
    log.info("mqtt published")

    setup_.p_power.publish_max_current(56)
    log.info("published max current")

    # for req_id_module, evse_manager in setup_.r_evse_manager.items():
    #     # log.error(f"mhhhhhh {dir(evse_manager)}")
    #     evse_manager.subscribe_limits(limits)
    #     signed_meter_value = evse_manager.call_get_signed_meter_value()
    #     log.error(f"signed meter value: {signed_meter_value}")

    for name, evse_manager in setup_.r_evse_manager.items():
        signed_meter_value = evse_manager.call_get_signed_meter_value()
        log.error(f"ONE signed meter value: {signed_meter_value}")

    # set_max_current = setup_.r_evse_manager.call_set_local_max_current(
    #     max_current=10)
    # log.error(f"set max current: {set_max_current}")
