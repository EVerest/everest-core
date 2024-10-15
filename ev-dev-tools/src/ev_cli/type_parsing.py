# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
Provide type parsing functionality.
author: kai-uwe.hermann@pionix.de
"""

from . import helpers

from pathlib import Path
from typing import Dict, List, Tuple


import stringcase


class TypeParser:
    """Provide generation of type headers from type definitions."""
    validators = None
    templates = None
    all_types = {}
    validated_type_defs = {}

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
    def does_type_exist(cls, type_url: str, json_type: str):
        """Checks if the referenced type exists"""
        if type_url not in TypeParser.all_types:
            TypeParser.all_types[type_url] = TypeParser.parse_type_url(type_url=type_url)
        type_dict = TypeParser.all_types[type_url]
        type_path = helpers.resolve_everest_dir_path('types' / type_dict['type_relative_path'].with_suffix('.yaml'))
        if not type_path or not type_path.exists():
            raise helpers.EVerestParsingException(
                '$ref: ' + type_url + f' referenced type file "{type_path}" does not exist.')
        if type_path not in TypeParser.validated_type_defs:
            TypeParser.validated_type_defs[type_path] = helpers.load_validated_type_def(
                type_path, TypeParser.validators['type'])

        if type_dict['type_name'] not in TypeParser.validated_type_defs[type_path]['types']:
            raise helpers.EVerestParsingException('$ref: ' + type_url + ' referenced type "' +
                                                  type_dict['type_name'] + f'" does not exist in type file "{type_path}".')

        type_schema = TypeParser.validated_type_defs[type_path]['types'][type_dict['type_name']]

        if json_type != type_schema['type']:
            raise helpers.EVerestParsingException('$ref: ' + type_url + ' referenced type "' +
                                                  type_dict['type_name'] + f'" in type file "{type_path}"' +
                                                  f' should be of type "{json_type}" but is of type: "' +
                                                  type_schema['type'] + '".')

    @classmethod
    def generate_tmpl_data_for_type(cls, type_with_namespace, type_def):
        """Generate template data based on the provided type and type definition."""
        helpers.parsed_enums.clear()
        helpers.parsed_types.clear()
        helpers.type_headers.clear()
        types = []
        enums = []

        for type_name, type_properties in type_def.get('types', {}).items():
            type_url = f'/{type_with_namespace["relative_path"]}#/{type_name}'
            TypeParser.all_types[type_url] = TypeParser.parse_type_url(type_url=type_url)
            try:
                (_type_info, enum_info) = helpers.extended_build_type_info(type_name, type_properties, type_file=True)
                if enum_info:
                    enums.append(enum_info)
            except helpers.EVerestParsingException as e:
                raise helpers.EVerestParsingException(f'Error parsing type {type_name}: {e}')

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

        type_headers = sorted(helpers.type_headers)

        # Remove the header itself from the includes.
        own_header = helpers.generate_header_for_type(type_with_namespace["relative_path"])
        type_headers = [header for header in type_headers if str(header) != str(own_header)]

        # sort types, so no forward declaration is necessary
        sorted_types: List = []
        for struct_type in types:
            insert_at: int = 0
            for dep_struct_type in struct_type['depends_on']:

                for i, _entry in enumerate(sorted_types):
                    # the new one depends on the current
                    if sorted_types[i]['name'] == dep_struct_type:
                        insert_at = max(insert_at, i + 1)
                        break

            sorted_types.insert(insert_at, struct_type)

        tmpl_data = {
            'info': {
                'type': type_with_namespace['namespace'],
                'desc': type_def['description'],
                'type_headers': type_headers,
            },
            'enums': enums,
            'types': sorted_types,
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
        namespaces.extend(type_with_namespace['relative_path'].parts)

        tmpl_data['info']['interface_name'] = f'{type_with_namespace["namespace"]}'
        tmpl_data['info']['namespace'] = namespaces
        tmpl_data['info']['hpp_guard'] = 'TYPES_' + helpers.snake_case(
            ''.join(type_with_namespace['uppercase_path'])).upper() + '_TYPES_HPP'

        types_parts['types'] = {
            'path': types_file,
            'content': TypeParser.templates['types.hpp'].render(tmpl_data),
            'last_mtime': last_mtime,
            'template_path': Path(TypeParser.templates['types.hpp'].filename),
            'printable_name': types_file.relative_to(output_path.parent)
        }

        return types_parts
