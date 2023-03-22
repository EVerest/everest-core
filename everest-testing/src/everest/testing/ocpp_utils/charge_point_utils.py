# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

from datetime import datetime
import OpenSSL.crypto as crypto
import logging


class HistoryMessage:
    def __init__(self, message, initiator) -> None:
        self.message = message
        self.initiator = initiator
        self.time = datetime.now()


class MessageHistory:
    def __init__(self) -> None:
        self.messages = []

    def add_received(self, message):
        self.messages.append(HistoryMessage(message, "Chargepoint"))

    def add_send(self, message):
        self.messages.append(HistoryMessage(message, "CSMS"))

    def log_history(self):
        for message in self.messages:
            time = message.time.strftime("%d-%m-%Y, %H:%M:%S")
            logging.info(f"{time} {message.initiator}: {message.message}")


def create_cert(serial_no, not_before, not_after, ca_cert, csr, ca_private_key):
    cert = crypto.X509()
    cert.set_serial_number(serial_no)
    cert.gmtime_adj_notBefore(0)
    cert.gmtime_adj_notAfter(not_after)
    cert.set_issuer(ca_cert.get_subject())
    cert.set_subject(csr.get_subject())
    cert.set_pubkey(csr.get_pubkey())
    cert.sign(ca_private_key, 'SHA256')

    return crypto.dump_certificate(crypto.FILETYPE_PEM, cert)
