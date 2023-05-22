# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
import logging
import netifaces

from everest.framework import log

from iso15118.secc.secc_settings import Config
from iso15118.shared.settings import shared_settings, set_ignoring_value_range
from iso15118.shared.utils import load_requested_auth_modes, load_requested_protocols



class EverestPyLoggingHandler(logging.Handler):

    def __init__(self):
        logging.Handler.__init__(self)

    def emit(self, record):
        msg = self.format(record)

        log_level: int = record.levelno
        if log_level == logging.CRITICAL:
            log.critical(msg)
        elif log_level == logging.ERROR:
            log.error(msg)
        elif log_level == logging.WARNING:
            log.warning(msg)
        # FIXME (aw): implicitely pipe everything with loglevel INFO into DEBUG
        else:
            log.debug(msg)

def setup_everest_logging():
    # remove all logging handler so that we'll have only our custom one
    # FIXME (aw): this is probably bad practice because if everyone does that, only the last one might survive
    logging.getLogger().handlers.clear()

    handler = EverestPyLoggingHandler()

    # NOTE (aw): the default formatting should be fine
    # formatter = logging.Formatter("%(levelname)s - %(name)s (%(lineno)d): %(message)s")
    # handler.setFormatter(formatter)

    logging.getLogger().addHandler(handler)

def choose_first_ipv6_local() -> str:
    for iface in netifaces.interfaces():
        if netifaces.AF_INET6 in netifaces.ifaddresses(iface):
            for netif_inet6 in netifaces.ifaddresses(iface)[netifaces.AF_INET6]:
                if 'fe80' in netif_inet6['addr']:
                    return iface

    log.warning('No necessary IPv6 link-local address was found!')
    return 'eth0'

def determine_network_interface(preferred_interface: str) -> str:
    if preferred_interface == "auto":
        return choose_first_ipv6_local()
    elif preferred_interface not in netifaces.interfaces():
        log.warning(f"The network interface {preferred_interface} was not found!")

    return preferred_interface

def patch_josev_config(josev_config: Config, everest_config: dict) -> None:

    josev_config.iface = determine_network_interface(everest_config['device'])

    josev_config.log_level = 'INFO'

    josev_config.enforce_tls = everest_config['enforce_tls']

    josev_config.free_cert_install_service = everest_config['free_cert_install_service']

    josev_config.allow_cert_install_service = everest_config['allow_cert_install_service']

    protocols = josev_config.default_protocols
    if not everest_config['supported_DIN70121']:
        protocols.remove('DIN_SPEC_70121')

    if not everest_config['supported_ISO15118_2']:
        protocols.remove('ISO_15118_2')

    if not everest_config['supported_ISO15118_20_AC']:
        protocols.remove('ISO_15118_20_AC')

    if not everest_config['supported_ISO15118_20_DC']:
        protocols.remove('ISO_15118_20_DC')

    josev_config.supported_protocols = load_requested_protocols(protocols)

    josev_config.use_cpo_backend = True
    log.info(f'Using CPO Backend: {josev_config.use_cpo_backend}')

    josev_config.supported_auth_options = load_requested_auth_modes(josev_config.default_auth_modes)

    josev_config.standby_allowed = False

    set_ignoring_value_range(everest_config['ignore_physical_values_limits'])

    if 'ciphersuites' in everest_config:
        josev_config.ciphersuites = everest_config['ciphersuites']

    if 'verify_contract_cert_chain' in everest_config:
        josev_config.verify_contract_cert_chain = everest_config['verify_contract_cert_chain']
