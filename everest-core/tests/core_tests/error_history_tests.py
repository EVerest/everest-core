# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from copy import deepcopy
from typing import Dict
import uuid
import pytest
import asyncio

from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.probe_module import ProbeModule

def assert_error(expected_error, error):
    assert expected_error['type'] == error['type']
    assert expected_error['sub_type'] == error['sub_type']
    assert expected_error['message'] == error['message']
    assert expected_error['severity'] == error['severity']
    assert expected_error['state'] == error['state']
    assert expected_error['description'] == error['description']
    assert expected_error['origin']['module_id'] == error['origin']['module_id']
    assert expected_error['origin']['implementation_id'] == error['origin']['implementation_id']

def assert_errors(expected_errors, errors):
    assert len(expected_errors) == len(errors)
    for exp_err in expected_errors:
        index = None
        for (i, err) in zip(range(len(errors)), errors):
            if exp_err['type'] == err['type'] and exp_err['sub_type'] == err['sub_type']:
                if index is not None:
                    assert False, f'Found multiple errors with uuid {exp_err["uuid"]}'
                index = i

        assert index is not None
        assert_error(exp_err, errors[index])

class ErrorHistoryModuleConfigurationStrategy(EverestConfigAdjustmentStrategy):
    def __init__(self, db_name):
        self.db_name = db_name
    def adjust_everest_configuration(self, everest_config: Dict) -> Dict:
        db_path = f'{self.db_name}.db'
        adjusted_config = deepcopy(everest_config)
        module_config = adjusted_config['active_modules']['error_history']
        module_config['config_implementation'] = {
            'error_history': {
                'database_path': db_path
            }
        }
        return adjusted_config

class UniqueDBNameStrategy(ErrorHistoryModuleConfigurationStrategy):
    def __init__(self):
        db_name = 'database_' + str(uuid.uuid4())
        super().__init__(db_name=db_name)
    def adjust_everest_configuration(self, everest_config: Dict) -> Dict:
        return super().adjust_everest_configuration(everest_config)

class TestErrorHistory:
    """
    Tests for the ErrorHistory module.
    """

    @pytest.mark.asyncio
    @pytest.mark.everest_core_config('config-test-error-history-module.yaml')
    @pytest.mark.everest_config_adaptions(UniqueDBNameStrategy())
    async def test_error_history(self, everest_core: EverestCore):
        """
        Test the ErrorHistory module.
        """
        everest_core.start(standalone_module='probe')

        probe_module = ProbeModule(everest_core.get_runtime_session())
        probe_module.start()

        call_args = {
            'filters': {}
        }
        result = await probe_module.call_command(
            'error_history',
            'get_errors',
            call_args
        )
        assert len(result) == 0

        err_args = {
            'type': 'test_errors/TestErrorA',
            'sub_type': '',
            'message': 'Test Error A',
            'severity': 'Low'
        }
        await probe_module.call_command(
            'test_error_handling',
            'raise_error',
            err_args
        )
        expected_error = {
            'type': err_args['type'],
            'sub_type': '',
            'message': err_args['message'],
            'severity': err_args['severity'],
            'state': 'Active',
            'description': 'Test error A',
            'origin': {
                'module_id': 'test_error_handling',
                'implementation_id': 'main'
            }
        }
        await asyncio.sleep(0.5)
        result = await probe_module.call_command(
            'error_history',
            'get_errors',
            call_args
        )
        assert len(result) == 1
        assert_error(expected_error, result[0])

        await probe_module.call_command(
            'test_error_handling',
            'clear_error',
            {
                'type': err_args['type'],
                'sub_type': err_args['sub_type'],
            }
        )
        expected_error['state'] = 'ClearedByModule'
        await asyncio.sleep(0.5)
        result = await probe_module.call_command(
            'error_history',
            'get_errors',
            call_args
        )
        assert_error(expected_error, result[0])

    @pytest.mark.asyncio
    @pytest.mark.everest_core_config('config-test-error-history-module.yaml')
    @pytest.mark.everest_config_adaptions(UniqueDBNameStrategy())
    async def test_error_history_filter(self, everest_core: EverestCore):
        """
        Test the ErrorHistory module with filters.
        """
        everest_core.start(standalone_module='probe')

        probe_module = ProbeModule(everest_core.get_runtime_session())
        probe_module.start()
        # insert 12 errors
        test_errors = [
            # index 0
            {
                'type': 'test_errors/TestErrorA',
                'sub_type': '',
                'message': 'Test Error A',
                'severity': 'Low',
                'description': 'Test error A',
                'state': 'Active',
                'origin': {
                    'module_id': 'test_error_handling',
                    'implementation_id': 'main'
                }
            },
            # index 1
            {
                'type': 'test_errors/TestErrorB',
                'sub_type': '',
                'message': 'Test Error B',
                'severity': 'Low',
                'description': 'Test error B',
                'state': 'Active',
                'origin': {
                    'module_id': 'test_error_handling',
                    'implementation_id': 'main'
                }
            },
        ]
        for err in test_errors:
            err_args = {
                'type': err['type'],
                'sub_type': err['sub_type'],
                'message': err['message'],
                'severity': err['severity']
            }
            await probe_module.call_command(
                'test_error_handling',
                'raise_error',
                err_args
            )
        await asyncio.sleep(0.5)

        # get all errors
        call_args = {
            'filters': {}
        }
        result = await probe_module.call_command(
            'error_history',
            'get_errors',
            call_args
        )
        assert_errors(test_errors, result)

        # get all errors with type TestErrorA
        call_args['filters'] = {
            'type_filter': 'test_errors/TestErrorA'
        }
        result = await probe_module.call_command(
            'error_history',
            'get_errors',
            call_args
        )
        assert_errors([test_errors[0]], result)

        # get all errors from module test_error_handling
        call_args['filters'] = {
            'origin_filter': {
                'module_id': 'test_error_handling',
                'implementation_id': 'main'
            }
        }
        result = await probe_module.call_command(
            'error_history',
            'get_errors',
            call_args
        )
        assert_errors(test_errors, result)

        # get all errors with severity Low
        call_args['filters'] = {
            'severity_filter': 'HIGH_GE'
        }
        result = await probe_module.call_command(
            'error_history',
            'get_errors',
            call_args
        )
        assert_errors([], result)

        # get all 'Active' errors
        call_args['filters'] = {
            'state_filter': 'Active'
        }
        result = await probe_module.call_command(
            'error_history',
            'get_errors',
            call_args
        )
        assert_errors(test_errors, result)

        for err in test_errors:
            await probe_module.call_command(
                'test_error_handling',
                'clear_error',
                {
                    'type': err['type'],
                    'sub_type': err['sub_type']
                }
            )
            err['state'] = 'ClearedByModule'
        await asyncio.sleep(0.5)

        call_args['filters'] = {
            'state_filter': 'ClearedByModule'
        }
        result = await probe_module.call_command(
            'error_history',
            'get_errors',
            call_args
        )
        assert_errors(test_errors, result)
