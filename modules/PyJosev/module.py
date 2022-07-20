from everestpy import log
import asyncio
import logging

import sys, os
JOSEV_WORK_DIR = str(os.getcwd() + '/josev')
sys.path.append(JOSEV_WORK_DIR)

from iso15118.secc import SECCHandler
from iso15118.secc.controller.simulator import SimEVSEController
from iso15118.shared.exificient_exi_codec import ExificientEXICodec

logger = logging.getLogger(__name__)

async def main():
    """
    Entrypoint function that starts the ISO 15118 code running on
    the SECC (Supply Equipment Communication Controller)
    """
    sim_evse_controller = await SimEVSEController.create()
    env_path = str(JOSEV_WORK_DIR + '/.env')
    await SECCHandler(
        exi_codec=ExificientEXICodec(), evse_controller=sim_evse_controller, env_path=env_path
    ).start()


def run():
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.debug("SECC program terminated manually")

setup_ = None

def pre_init(setup):
    global setup_
    setup_ = setup

def init():
    log.info(f"init")

def ready():
    log.info(f"ready")

    log.info("Josev is starting ...")
    run()
