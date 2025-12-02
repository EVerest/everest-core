# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
Provide error parsing functionality.
author: andreas.heinrich@pionix.de
"""

from typing import List, NamedTuple
from pathlib import Path
from . import helpers
import yaml, jsonschema

class ErrorDefinition(NamedTuple):
    """Error definition class."""
    namespace: str
    name: str
    description: str

class ErrorParser:
    """Error parser class."""
    validators = None
    error_definitions = {}

    @classmethod
    def load_error_definition_file(cls, path: Path):
        try:
            error_def = yaml.safe_load(path.read_text())
            ErrorParser.validators['error_declaration_list'].validate(error_def)
        except OSError as err:
            raise Exception(f'Could not open error definition file {err.filename}: {err.strerror}') from err
        except yaml.YAMLError as err:
            raise Exception(f'Could not parse error definition file {path}: {err}') from err
        except jsonschema.ValidationError as err:
            raise Exception(f'Error definition file {path} is not valid: {err}') from err

        namespace = path.stem
        if namespace in cls.error_definitions:
            raise Exception(f'Error definition namespace { namespace } already exists.')
        else:
            cls.error_definitions[namespace] = {}
        for entry in error_def['errors']:
            error = ErrorDefinition(namespace, entry['name'], entry['description'])
            if error.name in cls.error_definitions[error.namespace]:
                raise Exception(f'Error definition { error.namespace }/{ error.name } already exists.')
            cls.error_definitions[error.namespace][error.name] = error

    @classmethod
    def get_error_definition(cls, namespace: str, name: str) -> ErrorDefinition:
        """Get error definition for the provided namespace and name."""
        if namespace not in cls.error_definitions:
            path = helpers.resolve_everest_dir_path(f'errors/{ namespace }.yaml')
            cls.load_error_definition_file(path)
        if name not in cls.error_definitions[namespace]:
            raise Exception(f'Error definition { namespace }/{ name } does not exist.')
        return cls.error_definitions[namespace][name]

    @classmethod
    def get_error_definitions(cls, namespace: str) -> List[ErrorDefinition]:
        """Get error definitions for the provided namespace."""
        if namespace not in cls.error_definitions:
            path = helpers.resolve_everest_dir_path(f'errors/{ namespace }.yaml')
            cls.load_error_definition_file(path)
        result = []
        for error in cls.error_definitions[namespace].values():
            result.append(error)
        return result

    @classmethod
    def resolve_error_reference(cls, error_ref: str) -> List[ErrorDefinition]:
        """Resolve error reference."""
        ref_prefix = '/errors/'
        if not error_ref.startswith(ref_prefix):
            raise Exception(f'Error reference { error_ref } does not start with { ref_prefix }.')
        error_ref = error_ref[len(ref_prefix):]
        if '#/' in error_ref:
            namespace, name = error_ref.split('#/')
            result = []
            result.append(cls.get_error_definition(namespace, name))
            return result
        else:
            return cls.get_error_definitions(error_ref)
