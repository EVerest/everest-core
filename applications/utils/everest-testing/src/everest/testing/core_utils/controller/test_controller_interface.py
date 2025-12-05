# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

class TestController():

    """This abstract class defines methods that are used within the test cases
    and should be implemented by you for your specific chargepoint and test
    environment. It includes definitions for the simulated behavior and events of a
    chargepoint and an electric vehicle.
    """

    def start(self):
        """
        This method starts the chargepoint. This includes
        the connection of the OCPP client to the CSMS.
        """
        raise NotImplementedError()

    def stop(self):
        """
        This method stops the chargepoint (similiar to power off). This includes
        the disconnection of the OCPP client from the CSMS.
        """
        raise NotImplementedError()

    def plug_in(self, connector_id):
        """
        Plug in of an electric vehicle to the chargepoint.
        """
        raise NotImplementedError()

    def plug_in_ac_iso(self, payment_type, connector_id):
        """
        Plug in of an electric vehicle to the chargepoint using AC ISO15118.
        """
        raise NotImplementedError()

    def plug_in_dc_iso(self, payment_type, connector_id):
        """
        Plug in of an electric vehicle to the chargepoint using DC ISO15118.
        """
        raise NotImplementedError()

    def plug_out_iso(self, connector_id):
        """
        Plug out of an electric vehicle properly ending the ISO15118 session.
        """
        raise NotImplementedError()

    def plug_out(self):
        """
        Plug out of an electric vehicle from the chargepoint.
        """
        raise NotImplementedError()

    def swipe(self, token):
        """
        Swipe the given RFID card at the RFID reader of the chargepoint.
        """


    def connect_websocket(self):
        """
        Connect the OCPP client. This method is only used after a disconnect_websocket call
        in the tests.
        """
        raise NotImplementedError()

    def disconnect_websocket(self):
        """
        Disconnects the OCPP client from the CSMS. The chargepoint still is powered up.
        """
        raise NotImplementedError()

    def didoe_fail(self):
        """
        Produces an RCD Error.
        """
        raise NotImplementedError()

    def raise_error(self, error_string, connector_id):
        """
        Produces an error (default MREC6UnderVoltage).
        """
        raise NotImplementedError()

    def clear_error(self, error_string, connector_id):
        """
        Clears an error (default MREC6UnderVoltage).
        """
        raise NotImplementedError()
