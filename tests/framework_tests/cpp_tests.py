# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import logging
import pytest
import asyncio

from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.probe_module import ProbeModule

from dataclasses import dataclass
@dataclass
class ErrorHandlingTesterState:
    """
    Represents state of ErrorHandlingTester
    """
    len_probe_module_errors_TestErrorA: int = 0
    len_probe_module_errors_cleared_TestErrorA: int = 0
    len_probe_module_errors_TestErrorB: int = 0
    len_probe_module_errors_cleared_TestErrorB: int = 0
    len_probe_module_errors_TestErrorC: int = 0
    len_probe_module_errors_cleared_TestErrorC: int = 0
    len_probe_module_errors_TestErrorD: int = 0
    len_probe_module_errors_cleared_TestErrorD: int = 0

    len_test_error_handling_errors_TestErrorA: int = 0
    len_test_error_handling_errors_cleared_TestErrorA: int = 0
    len_test_error_handling_errors_TestErrorB: int = 0
    len_test_error_handling_errors_cleared_TestErrorB: int = 0
    len_test_error_handling_errors_all: int = 0
    len_test_error_handling_errors_cleared_all: int = 0
    len_test_error_handling_errors_global_all: int = 0
    len_test_error_handling_errors_cleared_global_all: int = 0

class ErrorHandlingTester:
    """
    Helper class to allow reusing testing mechanisms.
    """
    def __init__(self, probe_module: ProbeModule):
        self.__probe_module = probe_module

        self.test_error_handling = {
            'errors_TestErrorA': [],
            'errors_cleared_TestErrorA': [],
            'errors_TestErrorB': [],
            'errors_cleared_TestErrorB': [],
            'errors_all': [],
            'errors_cleared_all': [],
            'errors_global_all': [],
            'errors_cleared_global_all': [],
        }

        self.probe_module = {
            'errors_TestErrorA': [],
            'errors_cleared_TestErrorA': [],
            'errors_TestErrorB': [],
            'errors_cleared_TestErrorB': [],
            'errors_TestErrorC': [],
            'errors_cleared_TestErrorC': [],
            'errors_TestErrorD': [],
            'errors_cleared_TestErrorD': [],
        }

        self.__test_error_handling_conn_id = 'test_error_handling'
        self.__test_error_handling_not_req_conn_id = 'test_error_handling_not_req'
        self.__test_error_handling_ful = self.__probe_module._setup.connections[self.__test_error_handling_conn_id][0]
        self.__probe_module._mod.subscribe_variable(
            self.__test_error_handling_ful,
            'errors_subscribe_TestErrorA',
            self.__handle_errors_subscribe_TestErrorA
        )
        self.__probe_module._mod.subscribe_variable(
            self.__test_error_handling_ful,
            'errors_cleared_subscribe_TestErrorA',
            self.__handle_errors_cleared_subscribe_TestErrorA
        )
        self.__probe_module._mod.subscribe_variable(
            self.__test_error_handling_ful,
            'errors_subscribe_TestErrorB',
            self.__handle_errors_subscribe_TestErrorB
        )
        self.__probe_module._mod.subscribe_variable(
            self.__test_error_handling_ful,
            'errors_cleared_subscribe_TestErrorB',
            self.__handle_errors_cleared_subscribe_TestErrorB
        )
        self.__probe_module._mod.subscribe_variable(
            self.__test_error_handling_ful,
            'errors_subscribe_all',
            self.__handle_errors_subscribe_all
        )
        self.__probe_module._mod.subscribe_variable(
            self.__test_error_handling_ful,
            'errors_cleared_subscribe_all',
            self.__handle_errors_cleared_subscribe_all
        )
        self.__probe_module._mod.subscribe_variable(
            self.__test_error_handling_ful,
            'errors_subscribe_global_all',
            self.__handle_errors_subscribe_global_all
        )
        self.__probe_module._mod.subscribe_variable(
            self.__test_error_handling_ful,
            'errors_cleared_subscribe_global_all',
            self.__handle_errors_cleared_subscribe_global_all
        )

        self.__probe_module._mod.subscribe_error(
            self.__test_error_handling_ful,
            'test_errors/TestErrorA',
            self.__handle_test_errors_TestErrorA,
            self.__handle_test_errors_TestErrorA_cleared
        )
        self.__probe_module._mod.subscribe_error(
            self.__test_error_handling_ful,
            'test_errors/TestErrorB',
            self.__handle_test_errors_TestErrorB,
            self.__handle_test_errors_TestErrorB_cleared
        )
        self.__probe_module._mod.subscribe_error(
            self.__test_error_handling_ful,
            'test_errors/TestErrorC',
            self.__handle_test_errors_TestErrorC,
            self.__handle_test_errors_TestErrorC_cleared
        )
        self.__probe_module._mod.subscribe_error(
            self.__test_error_handling_ful,
            'test_errors/TestErrorD',
            self.__handle_test_errors_TestErrorD,
            self.__handle_test_errors_TestErrorD_cleared
        )

    # subscribe var handlers
    def __handle_errors_subscribe_TestErrorA(self, error):
        logging.debug(f'handle_test_errors_TestErrorA: {error}')
        self.test_error_handling['errors_TestErrorA'].append(error)
    def __handle_errors_cleared_subscribe_TestErrorA(self, error):
        logging.debug(f'handle_test_errors_TestErrorA_cleared: {error}')
        self.test_error_handling['errors_cleared_TestErrorA'].append(error)
    def __handle_errors_subscribe_TestErrorB(self, error):
        logging.debug(f'handle_test_errors_TestErrorB: {error}')
        self.test_error_handling['errors_TestErrorB'].append(error)
    def __handle_errors_cleared_subscribe_TestErrorB(self, error):
        logging.debug(f'handle_test_errors_TestErrorB_cleared: {error}')
        self.test_error_handling['errors_cleared_TestErrorB'].append(error)
    def __handle_errors_subscribe_all(self, error):
        logging.debug(f'handle_test_errors_all: {error}')
        self.test_error_handling['errors_all'].append(error)
    def __handle_errors_cleared_subscribe_all(self, error):
        logging.debug(f'handle_test_errors_all_cleared: {error}')
        self.test_error_handling['errors_cleared_all'].append(error)
    def __handle_errors_subscribe_global_all(self, error):
        logging.debug(f'handle_test_errors_global_all: {error}')
        self.test_error_handling['errors_global_all'].append(error)
    def __handle_errors_cleared_subscribe_global_all(self, error):
        logging.debug(f'handle_test_errors_global_all_cleared: {error}')
        self.test_error_handling['errors_cleared_global_all'].append(error)

    # subscribe error handlers
    def __handle_test_errors_TestErrorA(self, error):
        logging.debug(f'handle_test_errors_TestErrorA: {error}')
        self.probe_module['errors_TestErrorA'].append(error)
    def __handle_test_errors_TestErrorA_cleared(self, error):
        logging.debug(f'handle_test_errors_TestErrorA_cleared: {error}')
        self.probe_module['errors_cleared_TestErrorA'].append(error)
    def __handle_test_errors_TestErrorB(self, error):
        logging.debug(f'handle_test_errors_TestErrorB: {error}')
        self.probe_module['errors_TestErrorB'].append(error)
    def __handle_test_errors_TestErrorB_cleared(self, error):
        logging.debug(f'handle_test_errors_TestErrorB_cleared: {error}')
        self.probe_module['errors_cleared_TestErrorB'].append(error)
    def __handle_test_errors_TestErrorC(self, error):
        logging.debug(f'handle_test_errors_TestErrorC: {error}')
        self.probe_module['errors_TestErrorC'].append(error)
    def __handle_test_errors_TestErrorC_cleared(self, error):
        logging.debug(f'handle_test_errors_TestErrorC_cleared: {error}')
        self.probe_module['errors_cleared_TestErrorC'].append(error)
    def __handle_test_errors_TestErrorD(self, error):
        logging.debug(f'handle_test_errors_TestErrorD: {error}')
        self.probe_module['errors_TestErrorD'].append(error)
    def __handle_test_errors_TestErrorD_cleared(self, error):
        logging.debug(f'handle_test_errors_TestErrorD_cleared: {error}')
        self.probe_module['errors_cleared_TestErrorD'].append(error)

    # raise test error functions on probe module
    def probe_module_main_raise_error_a(self) -> (dict, str):
        """
        Raises an test error of type TestErrorA on the probe module (main)
        Returns the error args and the uuid of the error
        """
        err_object = {
            'type': 'test_errors/TestErrorA',
            'sub_type': '',
            'message': 'Test error message',
            'severity': 'Medium'
        }
        self.__probe_module._mod.raise_error(
            'main',
            err_object
        )
        return err_object
    def probe_module_main_raise_error_b(self) -> (dict, str):
        """
        Raises an test error of type TestErrorB on the probe modulee (main)
        Returns the error and the uuid of the error
        """
        err_object = {
            'type': 'test_errors/TestErrorB',
            'sub_type': '',
            'message': 'Test error message',
            'severity': 'Low'
        }
        self.__probe_module._mod.raise_error(
            'main',
            err_object
        )
        return err_object
    def probe_module_main_raise_error_c(self) -> (dict, str):
        """
        Raises an test error of type TestErrorC on the probe modulee (main)
        Returns the error and the uuid of the error
        """
        err_object = {
            'type': 'test_errors/TestErrorC',
            'sub_type': '',
            'message': 'Test error message',
            'severity': 'High'
        }
        self.__probe_module._mod.raise_error(
            'main',
            err_object
        )
        return err_object
    def probe_module_main_raise_error_d(self) -> (dict, str):
        """
        Raises an test error of type TestErrorD on the probe modulee (main)
        Returns the error and the uuid of the error
        """
        err_object = {
            'type': 'test_errors/TestErrorD',
            'sub_type': '',
            'message': 'Test error message',
            'severity': 'Medium'
        }
        self.__probe_module._mod.raise_error(
            'main',
            err_object
        )
        return err_object
    def probe_module_clear_error(self, type: str, sub_type: str):
        """
        Clears an error on the probe module (main)
        """
        self.__probe_module._mod.clear_error(
            'main',
            type,
            sub_type
        )
    # raise test error functions on test_error_handling module
    async def test_error_handling_raise_error_a(self) -> (dict, str):
        """
        Raises an test error of type TestErrorA on the test_error_handling module
        Returns the error and the uuid of the error
        """
        err_args = {
            'type': 'test_errors/TestErrorA',
            'sub_type': '',
            'message': 'Test error message',
            'severity': 'Medium'
        }
        await self.__probe_module.call_command(
            self.__test_error_handling_conn_id,
            'raise_error',
            err_args
        )
        return err_args
    async def test_error_handling_raise_error_b(self) -> (dict, str):
        """
        Raises an test error of type TestErrorB on the test_error_handling module
        Returns the error and the uuid of the error
        """
        err_args = {
            'type': 'test_errors/TestErrorB',
            'sub_type': '',
            'message': 'Test error message',
            'severity': 'Low'
        }
        await self.__probe_module.call_command(
            self.__test_error_handling_conn_id,
            'raise_error',
            err_args
        )
        return err_args
    async def test_error_handling_raise_error_c(self) -> (dict, str):
        """
        Raises an test error of type TestErrorC on the test_error_handling module
        Returns the error and the uuid of the error
        """
        err_args = {
            'type': 'test_errors/TestErrorC',
            'sub_type': '',
            'message': 'Test error message',
            'severity': 'High'
        }
        await self.__probe_module.call_command(
            self.__test_error_handling_conn_id,
            'raise_error',
            err_args
        )
        return err_args
    async def test_error_handling_raise_error_d(self) -> (dict, str):
        """
        Raises an test error of type TestErrorD on the test_error_handling module
        Returns the error and the uuid of the error
        """
        err_args = {
            'type': 'test_errors/TestErrorD',
            'sub_type': '',
            'message': 'Test error message',
            'severity': 'Medium'
        }
        await self.__probe_module.call_command(
            self.__test_error_handling_conn_id,
            'raise_error',
            err_args
        )
        return err_args
    async def test_error_handling_clear_a(self):
        """
        Clears all errors of type TestErrorA on the test_error_handling module
        """
        await self.__probe_module.call_command(
            self.__test_error_handling_conn_id,
            'clear_error',
            {
                'type': 'test_errors/TestErrorA',
                'sub_type': ''
            }
        )
    async def test_error_handling_clear_b(self):
        """
        Clears all errors of type TestErrorB on the test_error_handling module
        """
        await self.__probe_module.call_command(
            self.__test_error_handling_conn_id,
            'clear_error',
            {
                'type': 'test_errors/TestErrorB',
                'sub_type': ''
            }
        )
    async def test_error_handling_clear_c(self):
        """
        Clears all errors of type TestErrorC on the test_error_handling module
        """
        await self.__probe_module.call_command(
            self.__test_error_handling_conn_id,
            'clear_error',
            {
                'type': 'test_errors/TestErrorC',
                'sub_type': ''
            }
        )
    async def test_error_handling_clear_d(self):
        """
        Clears all errors of type TestErrorD on the test_error_handling module
        """
        await self.__probe_module.call_command(
            self.__test_error_handling_conn_id,
            'clear_error',
            {
                'type': 'test_errors/TestErrorD',
                'sub_type': ''
            }
        )
    async def test_error_handling_clear_all(self):
        """
        Clears all errors on the test_error_handling module
        """
        await self.__probe_module.call_command(
            self.__test_error_handling_conn_id,
            'clear_all_errors',
            {}
        )

    async def test_error_handling_not_req_raise_error_a(self) -> (dict, str):
        """
        Raises an test error of type TestErrorA on the test_error_handling_not_req module
        Returns the error and the uuid of the error
        """
        err_args = {
            'type': 'test_errors/TestErrorA',
            'sub_type': '',
            'message': 'Test error message',
            'severity': 'Medium'
        }
        await self.__probe_module.call_command(
            self.__test_error_handling_not_req_conn_id,
            'raise_error',
            err_args
        )
        return err_args

    def get_state(self) -> ErrorHandlingTesterState:
        state = ErrorHandlingTesterState(
            len_probe_module_errors_TestErrorA=len(self.probe_module['errors_TestErrorA']),
            len_probe_module_errors_cleared_TestErrorA=len(self.probe_module['errors_cleared_TestErrorA']),
            len_probe_module_errors_TestErrorB=len(self.probe_module['errors_TestErrorB']),
            len_probe_module_errors_cleared_TestErrorB=len(self.probe_module['errors_cleared_TestErrorB']),
            len_probe_module_errors_TestErrorC=len(self.probe_module['errors_TestErrorC']),
            len_probe_module_errors_cleared_TestErrorC=len(self.probe_module['errors_cleared_TestErrorC']),
            len_probe_module_errors_TestErrorD=len(self.probe_module['errors_TestErrorD']),
            len_probe_module_errors_cleared_TestErrorD=len(self.probe_module['errors_cleared_TestErrorD']),
            len_test_error_handling_errors_TestErrorA=len(self.test_error_handling['errors_TestErrorA']),
            len_test_error_handling_errors_cleared_TestErrorA=len(self.test_error_handling['errors_cleared_TestErrorA']),
            len_test_error_handling_errors_TestErrorB=len(self.test_error_handling['errors_TestErrorB']),
            len_test_error_handling_errors_cleared_TestErrorB=len(self.test_error_handling['errors_cleared_TestErrorB']),
            len_test_error_handling_errors_all=len(self.test_error_handling['errors_all']),
            len_test_error_handling_errors_cleared_all=len(self.test_error_handling['errors_cleared_all']),
            len_test_error_handling_errors_global_all=len(self.test_error_handling['errors_global_all']),
            len_test_error_handling_errors_cleared_global_all=len(self.test_error_handling['errors_cleared_global_all']),
        )
        return state
class TestErrorHandling:
    """
    Tests for error handling
    """
    @pytest.fixture
    def probe_module(self, everest_core: EverestCore):
        return ProbeModule(everest_core.get_runtime_session())

    @pytest.fixture
    def error_handling_tester(self, probe_module: ProbeModule):
        """
        Fixture to allow reusing testing mechanisms.
        """
        return ErrorHandlingTester(probe_module)

    @pytest.mark.everest_core_config('config-test-cpp-error-handling.yaml')
    @pytest.mark.asyncio
    async def test_raise_error(
        self,
        everest_core: EverestCore,
        error_handling_tester: ErrorHandlingTester
    ):
        """
        Tests that errors are raised correctly.
        The probe module triggers the TestErrorHandling module to raise an error,
        and then checks that the error is raised by subscribing to it.
        """
        everest_core.start(standalone_module='probe')
        err_args = await error_handling_tester.test_error_handling_raise_error_a()
        await asyncio.sleep(0.5)
        expected_state = ErrorHandlingTesterState(
            len_probe_module_errors_TestErrorA=1,
            len_test_error_handling_errors_global_all=1
        )
        assert error_handling_tester.get_state() == expected_state, 'State does not match expected state'
        recieved_error = error_handling_tester.probe_module['errors_TestErrorA'][0]
        assert recieved_error['type'] == err_args['type'], 'Received wrong error type'
        assert recieved_error['sub_type'] == err_args['sub_type'], 'Received wrong error sub type'
        assert recieved_error['message'] == err_args['message'], 'Received wrong error message'
        assert recieved_error['severity'] == err_args['severity'], 'Received wrong error severity'

    async def test_clear_errors_by_type(
        self,
        everest_core: EverestCore,
        error_handling_tester: ErrorHandlingTester
    ):
        """
        Tests that errors are cleared correctly.
        The probe module triggers the TestErrorHandling module to raise multiple errors,
        and then triggers the TestErrorHandling module to clear errors by type,
        and then checks that the correct errors are cleared.
        """

        everest_core.start(standalone_module='probe')
        await error_handling_tester.test_error_handling_raise_error_a()
        await error_handling_tester.test_error_handling_raise_error_a()
        await error_handling_tester.test_error_handling_raise_error_a()
        await error_handling_tester.test_error_handling_raise_error_b()
        await error_handling_tester.test_error_handling_raise_error_b()
        await error_handling_tester.test_error_handling_raise_error_c()
        await asyncio.sleep(0.5)
        await error_handling_tester.test_error_handling_clear_b()
        await asyncio.sleep(0.5)
        expected_state = ErrorHandlingTesterState(
            len_probe_module_errors_TestErrorA=3,
            len_probe_module_errors_TestErrorB=2,
            len_probe_module_errors_TestErrorC=1,
            len_probe_module_errors_cleared_TestErrorB=2,
            len_test_error_handling_errors_global_all=6,
            len_test_error_handling_errors_cleared_global_all=2
        )
        assert error_handling_tester.get_state() == expected_state, 'State does not match expected state'
        raised_errors_b = error_handling_tester.probe_module['errors_TestErrorB']
        cleared_errors_B = error_handling_tester.probe_module['errors_cleared_TestErrorB']
        for raised_error in raised_errors_b:
            assert raised_error in cleared_errors_B, 'Raised errors should be cleared'

    @pytest.mark.everest_core_config('config-test-cpp-error-handling.yaml')
    @pytest.mark.asyncio
    async def test_clear_all_errors(
        self,
        everest_core: EverestCore,
        error_handling_tester: ErrorHandlingTester
    ):
        """
        Tests that errors are cleared correctly.
        The probe module triggers the TestErrorHandling module to raise multiple errors,
        and then triggers the TestErrorHandling module to clear all errors,
        and then checks that all errors are cleared.
        """

        everest_core.start(standalone_module='probe')
        await error_handling_tester.test_error_handling_raise_error_a()
        await error_handling_tester.test_error_handling_raise_error_a()
        await error_handling_tester.test_error_handling_raise_error_a()
        await error_handling_tester.test_error_handling_raise_error_b()
        await error_handling_tester.test_error_handling_raise_error_b()
        await error_handling_tester.test_error_handling_raise_error_c()
        await asyncio.sleep(0.5)
        await error_handling_tester.test_error_handling_clear_all()
        await asyncio.sleep(0.5)
        expected_state = ErrorHandlingTesterState(
            len_probe_module_errors_TestErrorA=3,
            len_probe_module_errors_TestErrorB=2,
            len_probe_module_errors_TestErrorC=1,
            len_probe_module_errors_cleared_TestErrorA=3,
            len_probe_module_errors_cleared_TestErrorB=2,
            len_probe_module_errors_cleared_TestErrorC=1,
            len_test_error_handling_errors_global_all=6,
            len_test_error_handling_errors_cleared_global_all=6
        )
        assert error_handling_tester.get_state() == expected_state, 'State does not match expected state'
        raised_errors = error_handling_tester.probe_module['errors_TestErrorA']
        raised_errors.extend(error_handling_tester.probe_module['errors_TestErrorB'])
        raised_errors.extend(error_handling_tester.probe_module['errors_TestErrorC'])
        raised_errors.extend(error_handling_tester.probe_module['errors_TestErrorD'])
        cleared_errors = error_handling_tester.probe_module['errors_cleared_TestErrorA']
        cleared_errors.extend(error_handling_tester.probe_module['errors_cleared_TestErrorB'])
        cleared_errors.extend(error_handling_tester.probe_module['errors_cleared_TestErrorC'])
        cleared_errors.extend(error_handling_tester.probe_module['errors_cleared_TestErrorD'])
        for raised_error in raised_errors:
            assert raised_error in cleared_errors, 'Raised errors should be cleared'


    @pytest.mark.everest_core_config('config-test-cpp-error-handling.yaml')
    @pytest.mark.asyncio
    async def test_receive_req_error(
        self,
        everest_core: EverestCore,
        error_handling_tester: ErrorHandlingTester
    ):
        """
        The probe module raises an error of type TestErrorA on the main implementation,
        TestErrorHandling should be subscribed to this error type.
        Checks that the error is subscribed correctly.
        """

        everest_core.start(standalone_module='probe')
        err_arg, uuid = error_handling_tester.probe_module_main_raise_error_a()
        await asyncio.sleep(0.5)
        expected_state = ErrorHandlingTesterState(
            len_test_error_handling_errors_TestErrorA=1,
            len_test_error_handling_errors_all=1,
            len_test_error_handling_errors_global_all=1
        )
        assert error_handling_tester.get_state() == expected_state, 'State does not match expected state'
        raised_error = error_handling_tester.test_error_handling['errors_TestErrorA'][0]
        assert raised_error['type'] == err_arg['type'], 'Raised error type does not match expected error'
        assert raised_error['message'] == err_arg['message'], 'Raised error message does not match expected error'
        assert raised_error['severity'] == err_arg['severity'], 'Raised error severity does not match expected error'
        assert raised_error == error_handling_tester.test_error_handling['errors_all'][0], 'Equal error should be received by all'
        assert raised_error == error_handling_tester.test_error_handling['errors_global_all'][0], 'Equal error should be received by global all'

    @pytest.mark.everest_core_config('config-test-cpp-error-handling.yaml')
    @pytest.mark.asyncio
    async def test_receive_req_error_cleared(
        self,
        everest_core: EverestCore,
        error_handling_tester: ErrorHandlingTester
    ):
        """
        The probe module raises an error of type TestErrorA on the main implementation,
        then clears this error,
        TestErrorHandling should be subscribed to this error type.
        Checks that the error_cleared is subscribed correctly.
        """

        everest_core.start(standalone_module='probe')
        err_arg, uuid = error_handling_tester.probe_module_main_raise_error_a()
        await asyncio.sleep(0.5)
        error_handling_tester.probe_module_main_clear_error_by_uuid(uuid)
        await asyncio.sleep(0.5)
        expected_state = ErrorHandlingTesterState(
            len_test_error_handling_errors_TestErrorA=1,
            len_test_error_handling_errors_all=1,
            len_test_error_handling_errors_global_all=1,
            len_test_error_handling_errors_cleared_TestErrorA=1,
            len_test_error_handling_errors_cleared_all=1,
            len_test_error_handling_errors_cleared_global_all=1
        )
        assert error_handling_tester.get_state() == expected_state, 'State does not match expected state'
        raised_error = error_handling_tester.test_error_handling['errors_TestErrorA'][0]
        cleared_error = error_handling_tester.test_error_handling['errors_cleared_TestErrorA'][0]
        assert raised_error == cleared_error, 'Raised error does not match cleared error'
        assert cleared_error == error_handling_tester.test_error_handling['errors_all'][0], 'Equal error should be received by all'
        assert cleared_error == error_handling_tester.test_error_handling['errors_global_all'][0], 'Equal error should be received by global all'

    @pytest.mark.everest_core_config('config-test-cpp-error-handling.yaml')
    @pytest.mark.asyncio
    async def test_receive_req_not_sub_error(
        self,
        everest_core: EverestCore,
        error_handling_tester: ErrorHandlingTester
    ):
        """
        The probe module raises an error of type TestErrorC on the main implementation,
        TestErrorHandling shouldn't be subscribed to this error type.
        Checks that the error is subscribed correctly.
        """

        everest_core.start(standalone_module='probe')
        err_arg, uuid = error_handling_tester.probe_module_main_raise_error_c()
        await asyncio.sleep(0.5)
        expected_state = ErrorHandlingTesterState(
            len_test_error_handling_errors_all=1,
            len_test_error_handling_errors_global_all=1
        )
        assert error_handling_tester.get_state() == expected_state, 'State does not match expected state'
        raised_error = error_handling_tester.test_error_handling['errors_all'][0]
        assert raised_error['type'] == err_arg['type'], 'Raised error type does not match expected error'
        assert raised_error['message'] == err_arg['message'], 'Raised error message does not match expected error'
        assert raised_error['severity'] == err_arg['severity'], 'Raised error severity does not match expected error'
        assert raised_error == error_handling_tester.test_error_handling['errors_global_all'][0], 'Equal error should be received by global all'

    @pytest.mark.everest_core_config('config-test-cpp-error-handling.yaml')
    @pytest.mark.asyncio
    async def test_receive_req_not_sub_error_cleared(
        self,
        everest_core: EverestCore,
        error_handling_tester: ErrorHandlingTester
    ):
        """
        The probe module raises an error of type TestErrorC on the main implementation,
        then clears this error,
        TestErrorHandling shouldn't be subscribed to this error type.
        Checks that the error_cleared is subscribed correctly.
        """

        everest_core.start(standalone_module='probe')
        err_arg, uuid = error_handling_tester.probe_module_main_raise_error_c()
        await asyncio.sleep(0.5)
        error_handling_tester.probe_module_main_clear_error_by_uuid(uuid)
        await asyncio.sleep(0.5)
        expected_state = ErrorHandlingTesterState(
            len_test_error_handling_errors_all=1,
            len_test_error_handling_errors_global_all=1,
            len_test_error_handling_errors_cleared_all=1,
            len_test_error_handling_errors_cleared_global_all=1
        )
        assert error_handling_tester.get_state() == expected_state, 'State does not match expected state'
        raised_error = error_handling_tester.test_error_handling['errors_all'][0]
        cleared_error = error_handling_tester.test_error_handling['errors_cleared_all'][0]
        assert raised_error == cleared_error, 'Raised error does not match cleared error'
        assert cleared_error == error_handling_tester.test_error_handling['errors_global_all'][0], 'Equal error should be received by global all'


    @pytest.mark.everest_core_config('config-test-cpp-error-handling.yaml')
    @pytest.mark.asyncio
    async def test_receive_not_req_error(
        self,
        everest_core: EverestCore,
        error_handling_tester: ErrorHandlingTester
    ):
        """
        The probe modules triggers the test_error_handling_not_req module
        to raise an error of type TestErrorA. This implementation is not required
        by TestErrorHandling, so it shouldn't be subscribed to this error type.
        Checks that the error is subscribed correctly.
        """

        everest_core.start(standalone_module='probe')
        err_arg, uuid = await error_handling_tester.test_error_handling_not_req_raise_error_a()
        await asyncio.sleep(0.5)
        expected_state = ErrorHandlingTesterState(
            len_test_error_handling_errors_global_all=1
        )
        assert error_handling_tester.get_state() == expected_state, 'State does not match expected state'
        raised_error = error_handling_tester.test_error_handling['errors_global_all'][0]
        assert raised_error['type'] == err_arg['type'], 'Raised error type does not match expected error'
        assert raised_error['message'] == err_arg['message'], 'Raised error message does not match expected error'
        assert raised_error['severity'] == err_arg['severity'], 'Raised error severity does not match expected error'

    @pytest.mark.everest_core_config('config-test-cpp-error-handling.yaml')
    @pytest.mark.asyncio
    async def test_receive_not_req_error_cleared(
        self,
        everest_core: EverestCore,
        error_handling_tester: ErrorHandlingTester
    ):
        """
        The probe modules triggers the test_error_handling_not_req module
        to raise an error of type TestErrorA.
        Then triggers the test_error_handling_not_req module to clear this error.
        This implementation is not required
        by TestErrorHandling, so it shouldn't be subscribed to this error type.
        Checks that the error is subscribed correctly.
        """

        everest_core.start(standalone_module='probe')
        err_arg, uuid = await error_handling_tester.test_error_handling_not_req_raise_error_a()
        await asyncio.sleep(0.5)
        await error_handling_tester.test_error_handling_not_req_clear_error_by_uuid(uuid)
        await asyncio.sleep(0.5)
        expected_state = ErrorHandlingTesterState(
            len_test_error_handling_errors_global_all=1,
            len_test_error_handling_errors_cleared_global_all=1
        )
        assert error_handling_tester.get_state() == expected_state, 'State does not match expected state'
        raised_error = error_handling_tester.test_error_handling['errors_global_all'][0]
        cleared_error = error_handling_tester.test_error_handling['errors_cleared_global_all'][0]
        assert raised_error == cleared_error, 'Raised error does not match cleared error'
