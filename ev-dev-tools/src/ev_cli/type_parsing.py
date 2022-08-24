# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#
"""
Provide type parsing functionality.
author: kai-uwe.hermann@pionix.de
"""

from . import helpers

from pathlib import Path
from typing import Dict, Tuple


import stringcase


class TypeParser:
    """Provide generation of type headers from type definitions."""
    validators = None
    templates = None
    all_types = {}

    @classmethod
    def parse_type_url(cls, type_url: str) -> Dict:
        """Parse a global type URL in the following format /filename#/typename."""

        type_dict = {
            'type_relative_path': None,
            'namespaced_type': None,
            'header_file': None,
            'type_name': None
        }
        if not type_url.startswith('/'):
            raise Exception('type_url: ' + type_url + ' needs to start with a "/".')
        if '#/' not in type_url:
            raise Exception('type_url: ' + type_url + ' needs to refer to a specific type with "#/TypeName".')
        type_relative_path, prop_type = type_url.split('#/')
        type_relative_path = Path(type_relative_path[1:])

        namespaced_type = 'types::' + '::'.join(type_relative_path.parts) + f'::{prop_type}'
        type_dict['type_relative_path'] = type_relative_path
        type_dict['namespaced_type'] = namespaced_type
        type_dict['type_name'] = prop_type

        return type_dict

    @classmethod
    def generate_tmpl_data_for_type(cls, type_with_namespace, type_def):
        """Generate template data based on the provided type and type definition."""
        helpers.parsed_enums.clear()
        helpers.parsed_types.clear()
        helpers.type_headers.clear()
        types = []
        enums = []
        type_headers = []

        for type_name, type_properties in type_def.get('types', {}).items():
            type_url = f'/{type_with_namespace["relative_path"]}#/{type_name}'
            TypeParser.all_types[type_url] = TypeParser.parse_type_url(type_url=type_url)
            (_type_info, enum_info) = helpers.extended_build_type_info(type_name, type_properties, type_file=True)
            if enum_info:
                enums.append(enum_info)

        for parsed_enum in helpers.parsed_enums:
            enum_info = {
                'name': parsed_enum['name'],
                'description': parsed_enum['description'],
                'enum_type': stringcase.capitalcase(parsed_enum['name']),
                'enum': parsed_enum['enums']
            }
            enums.append(enum_info)

        for parsed_type in helpers.parsed_types:
            parsed_type['name'] = stringcase.capitalcase(parsed_type['name'])
            types.append(parsed_type)

        for type_header in helpers.type_headers:
            type_headers.append(type_header)

        tmpl_data = {
            'info': {
                'type': type_with_namespace['namespace'],
                'desc': type_def['description'],
                'type_headers': type_headers,
            },
            'enums': enums,
            'types': types,
        }

        return tmpl_data

    @classmethod
    def load_type_definition(cls, type_path: Path):
        """Load a type definition from the provided path and check its last modification time."""
        type_def = helpers.load_validated_type_def(type_path, TypeParser.validators['type'])

        last_mtime = type_path.stat().st_mtime

        return type_def, last_mtime

    @classmethod
    def generate_type_info(cls, type_with_namespace, all_types) -> Tuple:
        """Generate type template data."""
        try:
            type_def, last_mtime = TypeParser.load_type_definition(type_with_namespace['path'])
        except Exception as e:
            if not all_types:
                raise
            else:
                print(f'Ignoring type {type_with_namespace["namespace"]} with reason: {e}')
                return

        tmpl_data = TypeParser.generate_tmpl_data_for_type(type_with_namespace, type_def)

        return (tmpl_data, last_mtime)

    @classmethod
    def generate_type_headers(cls, type_with_namespace, all_types, output_dir):
        """Render template data to generate type headers."""
        tmpl_data, last_mtime = TypeParser.generate_type_info(type_with_namespace, all_types)

        types_parts = {'types': None}

        output_path = output_dir / type_with_namespace['relative_path']
        types_file = output_path.with_suffix('.hpp')
        output_path = output_path.parent
        output_path.mkdir(parents=True, exist_ok=True)

        namespaces = ['types']
        namespaces.extend(type_with_namespace["relative_path"].parts)

        tmpl_data['info']['interface_name'] = f'{type_with_namespace["namespace"]}'
        tmpl_data['info']['namespace'] = namespaces
        tmpl_data['info']['hpp_guard'] = 'TYPES_' + helpers.snake_case(
            ''.join(type_with_namespace["uppercase_path"])).upper() + '_TYPES_HPP'

        types_parts['types'] = {
            'path': types_file,
            'content': TypeParser.templates['types.hpp'].render(tmpl_data),
            'last_mtime': last_mtime,
            'printable_name': types_file.relative_to(output_path.parent)
        }

        return types_parts
