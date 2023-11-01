# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#
"""
author: aw@pionix.de
FIXME (aw): Module documentation.
"""

from .type_parsing import TypeParser

from pathlib import Path
import shutil
import subprocess
import re
from typing import Dict, List, Tuple
import keyword

import json
import jsonschema

import yaml

from uuid import uuid4

import stringcase


everest_dirs: List[Path] = []


class EVerestParsingException(SystemExit):
    pass


def snake_case(word: str) -> str:
    """Convert capital case to snake case
    Only alphanumerical characters are allowed.  Only inserts camelcase
    between a consecutive lower and upper alphabetical character and
    lowers first letter
    """

    out = ''
    if len(word) == 0:
        return out
    cur_char: str = ''
    for i in range(len(word)):
        if i == 0:
            cur_char = word[i]
            if not cur_char.isalnum():
                raise Exception('Non legal character in: ' + word)
            out += cur_char.lower()
            continue
        last_char: str = cur_char
        cur_char = word[i]
        if (last_char.islower() and last_char.isalpha() and cur_char.isupper() and cur_char.isalpha):
            out += '_'
        if not cur_char.isalnum():
            out += '_'
        else:
            out += cur_char.lower()

    return out


def create_dummy_result(json_type) -> str:
    def primitive_to_sample_value(type):
        if type == 'boolean':
            return 'true'
        elif type == 'integer':
            return '42'
        elif type == 'number':
            return '3.14'
        elif type == 'string':
            return '"everest"'
        elif type == 'object':
            return '{}'
        else:
            raise Exception(f'This json type "{type}" is not known or not implemented')

    if isinstance(json_type, list):
        return '{}'  # default initialization for variant
    else:
        return primitive_to_sample_value(json_type)


cpp_type_map = {
    "null": "std::nullptr_t",  # FIXME (aw): who gets the null, json? or the variant
    "integer": "int",
    "number": "double",
    "string": "std::string",
    "boolean": "bool",
    "array": "Array",
    "object": "Object",
}


def clang_format(config_file_path, file_info):
    # check if we handle cpp and hpp files
    if not file_info['path'].suffix in ('.hpp', '.cpp'):
        return

    clang_format_path = shutil.which('clang-format')
    if clang_format_path is None:
        raise RuntimeError('Could not find clang-format executable - needed when passing clang-format config file')

    config_file_path = Path(config_file_path)
    if not config_file_path.is_dir():
        raise RuntimeError(f'Supplied directory for the clang-format file ({config_file_path}) does not exist')

    if not (config_file_path / '.clang-format').exists():
        raise RuntimeError(f'Supplied directory for the clang-format file '
                           f'({config_file_path}) does not contain a .clang-format file')

    content = file_info['content']

    run_parms = {'capture_output': True, 'cwd': config_file_path, 'encoding': 'utf-8', 'input': content}

    format_cmd = subprocess.run([clang_format_path, '--style=file'], **run_parms)

    if format_cmd.returncode != 0:
        raise RuntimeError(f'clang-format failed with:\n{format_cmd.stderr}')

    file_info['content'] = format_cmd.stdout


def resolve_everest_dir_path(postfix):
    resolved_path = None
    for everest_dir in everest_dirs:
        path = everest_dir / postfix
        if path.exists():
            resolved_path = path
            break

    if not resolved_path:
        raise EVerestParsingException(
            f'Could not resolve "{postfix}" in any of the provided everest-dir ({everest_dirs}).')

    return resolved_path


def build_type_info(name, json_type):
    ti = {
        'name': name,
        'is_variant': False,
        'cpp_type': None,
        'json_type': json_type
    }

    if isinstance(json_type, list):
        ti['is_variant'] = True
        ti['cpp_type'] = [cpp_type_map[e] for e in json_type if e != 'null']
        ti['cpp_type'].sort()  # sort, so template generation might get reduced
        # prepend boost::blank if type 'null' exists, so the variant
        # gets default initialized with blank
        if 'null' in json_type:
            ti['cpp_type'].insert(0, cpp_type_map['null'])
    else:
        ti['cpp_type'] = cpp_type_map[json_type]

    return ti


type_headers = set()
parsed_types: List = []
parsed_enums: List = []
current_defs: Dict = {}

format_types = dict()
# format_types['date-time'] = 'DateTime'


def object_exists(name: str) -> bool:
    """Check if an object already exists."""
    for el in parsed_types:
        if el['name'] == name:
            return True

    return False


def add_enum_type(name: str, enums: Tuple[str], description: str):
    """Add enum type to parsed_types."""
    for el in parsed_enums:
        if el['name'] == name:
            raise Exception('Warning: enum ' + name + ' already exists')
    parsed_enums.append({
        'name': name,
        'enums': enums,
        'description': description
    })


def parse_ref(ref: str, prop_type, prop_info: Dict) -> Tuple[str, dict]:
    if ref not in TypeParser.all_types:
        TypeParser.all_types[ref] = TypeParser.parse_type_url(type_url=ref)
    type_dict = TypeParser.all_types[ref]

    type_path = resolve_everest_dir_path('types' / type_dict['type_relative_path'] .with_suffix('.yaml'))
    if not type_path or not type_path.exists():
        raise EVerestParsingException('$ref: ' + ref + f' referenced type file "{type_path} does not exist.')

    (td, _mod) = TypeParser.load_type_definition(type_path)
    if 'types' in td and type_dict['type_name'] in td['types']:
        local_type_info = td['types'][type_dict['type_name']]
        if local_type_info['type'] == 'string' and 'enum' in local_type_info:
            prop_info['enum'] = True
    prop_type = type_dict['namespaced_type']
    prop_info['prop']['type'] = prop_type
    prop_info['type_dict'] = type_dict

    path = Path('generated/types') / \
        type_dict['type_relative_path'].with_suffix('.hpp')
    type_headers.add(path.as_posix())

    return (prop_type, prop_info)


def parse_property(prop_name: str, prop: Dict, depends_on: List[str], type_file: bool) -> Tuple[str, dict]:
    """Determine type of property and proceed with it.
    In case it is a $ref, look it up in the TypeParser
    Currently, the following property types are supported:
    - string (and enum as a special case)
    - integer
    - number
    - boolean
    - array
    - object (will be parsed recursivly)
    """

    prop_type = None
    prop_info = {
        'description': prop.get('description', 'TODO: description'),
        'prop': prop,
        'enum': False
    }
    if '$ref' in prop:
        return parse_ref(prop['$ref'], prop_type, prop_info)

    if 'type' not in prop:
        raise EVerestParsingException(f'{prop_name} does not contain a type property')

    if prop['type'] == 'string':
        if 'enum' in prop and type_file:
            prop_type = stringcase.capitalcase(prop_name)
            add_enum_type(prop_type, prop['enum'], prop_info['description'])
        elif 'format' in prop:
            if prop['format'] in format_types:
                prop_type = format_types[prop['format']]
            else:
                # unsupported format type
                prop_type = 'std::string'
                prop_info['unsupported_format'] = True
        else:
            prop_type = 'std::string'
    elif prop['type'] == 'integer':
        prop_type = 'int32_t'
    elif prop['type'] == 'number':
        prop_type = 'float'
    elif prop['type'] == 'boolean':
        prop_type = 'bool'
    elif prop['type'] == 'array':
        if 'items' in prop:
            prop_type = 'std::vector<' + parse_property(prop_name, prop['items'], depends_on, type_file)[0] + '>'
        else:
            raise EVerestParsingException(f'Property items of array {prop_name} does not contain a type property')
    elif prop['type'] == 'object':
        prop_type = stringcase.capitalcase(prop_name)
        depends_on.append(prop_type)
        if not object_exists(prop_type):
            parse_object(prop_type, prop, type_file)
    else:
        raise Exception('Unknown type: ' + prop['type'])

    return (prop_type, prop_info)


def parse_object(ob_name: str, json_schema: Dict, type_file: bool):
    """Parse a JSON object.
    Iterates over the properties of this object, parses their type
    and puts these information into the global dict parsed_types.
    """

    ob_dict = {'name': ob_name, 'properties': [], 'depends_on': []}
    parsed_types.insert(0, ob_dict)

    if 'properties' not in json_schema:
        # object has no properties, probably not a complex object
        if '$ref' in json_schema:
            if json_schema['$ref'] not in TypeParser.all_types:
                TypeParser.all_types[json_schema['$ref']] = TypeParser.parse_type_url(type_url=json_schema['$ref'])
            type_dict = TypeParser.all_types[json_schema['$ref']]

            type_path = resolve_everest_dir_path('types' / type_dict['type_relative_path'].with_suffix('.yaml'))
            if not type_path or not type_path.exists():
                raise EVerestParsingException(
                    '$ref: ' + json_schema['$ref'] + f' referenced type file "{type_path} does not exist.')
            TypeParser.does_type_exist(type_url=json_schema['$ref'], json_type=json_schema['type'])

            prop_type = type_dict['namespaced_type']
            ob_dict['name'] = prop_type
            path = Path('generated/types') / \
                type_dict['type_relative_path'].with_suffix('.hpp')
            type_headers.add(path.as_posix())
            return ob_dict
        return

    if not type_file:
        return

    for prop_name, prop in json_schema['properties'].items():
        if not prop_name.isidentifier() or keyword.iskeyword(prop_name):
            raise Exception(prop_name + ' can\'t be used as an identifier!')
        (prop_type, prop_info) = parse_property(prop_name, prop, ob_dict['depends_on'], type_file)
        ob_dict['properties'].append({
            'name': prop_name,
            'json_name': prop_name,
            'type': prop_type,
            'info': prop_info,
            'enum': 'enum' in prop or prop_info['enum'],
            'required': prop_name in json_schema.get('required', {}),
        })

    ob_dict['properties'].sort(key=lambda x: x.get('required'), reverse=True)

    return ob_dict


def extended_build_type_info(name: str, info: dict, type_file=False) -> Tuple[dict, dict]:
    """Extend build_type_info with enum and object type handling."""
    type_info = build_type_info(name, info['type'])
    enum_info = None

    if type_info['json_type'] == 'string':
        if 'enum' in info and type_file:
            enum_info = {
                'name': name,
                'description': info.get('description', 'TODO: description'),
                'enum_type': stringcase.capitalcase(name),
                'enum': info['enum']
            }

            type_info['enum_type'] = enum_info['enum_type']
        elif '$ref' in info:
            if info['$ref'] not in TypeParser.all_types:
                TypeParser.all_types[info['$ref']] = TypeParser.parse_type_url(type_url=info['$ref'])
            type_dict = TypeParser.all_types[info['$ref']]

            type_path = resolve_everest_dir_path('types' / type_dict['type_relative_path'] .with_suffix('.yaml'))
            if not type_path or not type_path.exists():
                raise EVerestParsingException('$ref: ' + info['$ref'] +
                                              f' referenced type file "{type_path} does not exist.')

            (td, _mod) = TypeParser.load_type_definition(type_path)
            if 'types' in td and type_dict['type_name'] in td['types']:
                local_type_info = td['types'][type_dict['type_name']]
                if local_type_info['type'] == 'string' and 'enum' in local_type_info:
                    enum_info = {
                        'name': name,
                        'description': local_type_info.get('description', 'TODO: description'),
                        'enum_type': type_dict['namespaced_type'],
                        'enum': local_type_info['enum']
                    }

                    type_info['enum_type'] = enum_info['enum_type']
            path = Path('generated/types') / \
                type_dict['type_relative_path'].with_suffix('.hpp')
            type_headers.add(path.as_posix())
    elif type_info['json_type'] == 'object':
        try:
            ob = parse_object(name, info, type_file)
            if ob and 'name' in ob:
                type_info['object_type'] = ob['name']
        except EVerestParsingException as e:
            raise EVerestParsingException(f'Error parsing object {name}: {e}')
    elif type_info['json_type'] == 'array':
        if '$ref' in info['items']:
            if info['items']['$ref'] not in TypeParser.all_types:
                TypeParser.all_types[info['items']['$ref']] = TypeParser.parse_type_url(type_url=info['items']['$ref'])
            type_dict = TypeParser.all_types[info['items']['$ref']]

            type_path = resolve_everest_dir_path('types' / type_dict['type_relative_path'] .with_suffix('.yaml'))
            if not type_path or not type_path.exists():
                raise EVerestParsingException(
                    '$ref: ' + info['items']['$ref'] + f' referenced type file "{type_path} does not exist.')

            (td, _mod) = TypeParser.load_type_definition(type_path)
            if 'types' in td and type_dict['type_name'] in td['types']:
                local_type_info = td['types'][type_dict['type_name']]
                if 'enum' in local_type_info:
                    type_info['array_type_contains_enum'] = True
                type_info['array_type'] = type_dict['namespaced_type']
            path = Path('generated/types') / \
                type_dict['type_relative_path'].with_suffix('.hpp')
            type_headers.add(path.as_posix())

    return (type_info, enum_info)


def load_validators(schema_path: Path):
    # FIXME (aw): we should also patch the schemas like in everest-framework
    validators = {}
    for validator, filename in zip(
        ['interface', 'module', 'config', 'type'],
            ['interface', 'manifest', 'config', 'type']):
        try:
            schema = yaml.safe_load((schema_path / f'{filename}.yaml').read_text())
            jsonschema.Draft7Validator.check_schema(schema)
            validators[validator] = jsonschema.Draft7Validator(schema)
        except OSError as err:
            print(f'Could not open schema file {err.filename}: {err.strerror}')
            exit(1)
        except jsonschema.SchemaError as err:
            print(f'Schema error in schema file {filename}.yaml')
            raise
        except yaml.YAMLError as err:
            raise Exception(f'Could not parse interface definition file {schema_path}') from err

    return validators


def load_validated_interface_def(if_def_path: Path, validator):
    if_def = {}
    try:
        if_def = yaml.safe_load(if_def_path.read_text())
        # validating interface
        validator.validate(if_def)
        # validate var/cmd subparts
        if "vars" in if_def:
            for _var_name, var_def in if_def["vars"].items():
                jsonschema.Draft7Validator.check_schema(var_def)
        if "cmds" in if_def:
            for _cmd_name, cmd_def in if_def["cmds"].items():
                if "arguments" in cmd_def:
                    for _arg_name, arg_def in cmd_def["arguments"].items():
                        jsonschema.Draft7Validator.check_schema(arg_def)
                if "result" in cmd_def:
                    jsonschema.Draft7Validator.check_schema(cmd_def["result"])
    except OSError as err:
        raise Exception(f'Could not open interface definition file {err.filename}: {err.strerror}') from err
    except jsonschema.ValidationError as err:
        raise Exception(f'Validation error in interface definition file {if_def_path}: {err}') from err
    except yaml.YAMLError as err:
        raise Exception(f'Could not parse interface definition file {if_def_path}') from err

    return if_def


def load_validated_type_def(type_def_path: Path, validator):
    """Load a type definition from the provided path and validate it with the provided validator."""

    try:
        type_def = yaml.safe_load(type_def_path.read_text())
        # validating type definition
        validator.validate(type_def)

        return type_def
    except OSError as err:
        raise Exception(f'Could not open type definition file {err.filename}: {err.strerror}') from err
    except jsonschema.ValidationError as err:
        raise Exception(f'Validation error in type definition file {type_def_path}') from err
    except yaml.YAMLError as err:
        raise Exception(f'Could not parse interface definition file {type_def_path}') from err

    return type_def


def load_validated_module_def(module_path: Path, validator):
    try:
        module_def = yaml.safe_load(module_path.read_text())
        validator.validate(module_def)
    except OSError as err:
        raise Exception(f'Could not open type definition file {err.filename}: {err.strerror}') from err
    except jsonschema.ValidationError as err:
        raise Exception(f'Validation error in module definition file {module_path}') from err
    except yaml.YAMLError as err:
        raise Exception(f'Could not parse interface definition file {module_path}') from err

    return module_def


def generate_some_uuids(count):
    for i in range(count):
        print(uuid4())


def yaml2json(yaml_file: Path, json_file: Path):
    if not yaml_file.exists():
        print(f'The input file ({yaml_file}) does not exist')
        exit(1)

    with open(yaml_file, 'r') as yaml_content:
        content_as_dict = yaml.safe_load(yaml_content)

    with open(json_file, 'w') as json_content:
        json.dump(content_as_dict, json_content, indent=2)


def json2yaml(json_file: Path, yaml_file: Path):
    if not json_file.exists():
        print(f'The input file ({json_file}) does not exist')
        exit(1)

    with open(json_file, 'r') as json_content:
        content_as_dict = json.load(json_content)

    with open(yaml_file, 'w') as yaml_content:
        yaml.safe_dump(content_as_dict, yaml_content, indent=2, sort_keys=False, width=120)


def __check_for_match(blocks_def, line, line_no, file_path):
    match = re.search(blocks_def['regex_str'], line)
    if not match:
        return None

    # mb = match_block
    mb = {
        'id': match.group('uuid'),
        'version': match.group('version'),
        'tag': blocks_def['format_str'].format(
            uuid=match.group('uuid'),
            version=match.group('version')
        )
    }

    # check if uuid and version exists
    if blocks_def['version'] != mb['version']:
        raise ValueError(
            f'Error while parsing {file_path}:\n'
            f'  matched line {line_no}: {line}\n'
            f'  contains version "{mb["version"]}", which is different from the blocks definition version "{blocks_def["version"]}"'
        )

    for block, block_info in blocks_def['definitions'].items():
        if block_info['id'] != mb['id']:
            continue

        mb['name'] = block
        mb['block'] = block_info

    if not 'block' in mb:
        raise ValueError(
            f'Error while parsing {file_path}:\n'
            f'  matched line {line_no}: {line}\n'
            f'  contains uuid "{mb["id"]}", which doesn\'t exist in the block definition'
        )

    return mb


def generate_tmpl_blocks(blocks_def, file_path=None):

    tmpl_block = {}
    for block_name, block_def in blocks_def['definitions'].items():
        tmpl_block[block_name] = {
            'tag': blocks_def['format_str'].format(
                uuid=block_def['id'],
                version=blocks_def['version']
            ),
            'content': block_def['content'],
            'first_use': True
        }

    if not file_path:
        return tmpl_block

    try:
        file_data = file_path.read_text()
    except OSError as err:
        print(f'Could not open file {err.filename} for parsing blocks: {err.strerror}')
        exit(1)

    line_no = 0
    matched_block = None
    content = None

    for line in file_data.splitlines(True):
        line_no += 1

        if not matched_block:
            matched_block = __check_for_match(blocks_def, line.rstrip(), line_no, file_path)
            content = None
            continue

        if (line.strip() == matched_block['tag']):
            if (content):
                tmpl_block[matched_block['name']]['content'] = content.rstrip()
                tmpl_block[matched_block['name']]['first_use'] = False
            matched_block = None
        else:
            content = (content + line) if content else line

    if matched_block:
        raise ValueError(
            f'Error while parsing {file_path}:\n'
            f'  matched tag line {matched_block["tag"]}\n'
            f'  could not find closing tag'
        )

    return tmpl_block


def load_tmpl_blocks(blocks_def, file_path, update):
    if update and file_path.exists():
        return generate_tmpl_blocks(blocks_def, file_path)
    else:
        return generate_tmpl_blocks(blocks_def)


def __show_diff_for(file_info):
    diff_path = shutil.which('diff')
    if diff_path == None:
        raise Exception('Can\'t generate diff, because "diff" executable not found')

    file_path = file_info['path']

    diff_ignore = ''
    if file_path.suffix in ('.hpp', '.cpp'):
        diff_ignore = '^//.*'
    elif file_path.name == 'CMakeLists.txt':
        diff_ignore = '^#.*'
    diff_ignore_args = ['-I', diff_ignore] if diff_ignore else []

    run_parms = {'input': file_info['content'], 'capture_output': True, 'encoding': 'utf-8'}

    diff = subprocess.run([
        diff_path,
        '-ruN',
        *diff_ignore_args,
        '--label', file_info['printable_name'],
        '--color=always',
        file_path,
        '-'
    ], **run_parms).stdout
    if diff:
        print(diff)


def filter_mod_files(only, mod_files):
    if not only:
        return

    filter_files = set([c.strip() for c in only.split(',')])
    not_filtered_files = filter_files.copy()

    # first check if all selected file filters are valid
    for category_files in mod_files.values():
        for file_info in category_files:
            if file_info['abbr'] in not_filtered_files:
                not_filtered_files.remove(file_info['abbr'])

    if not_filtered_files:
        raise Exception(f'Unknown file filters for --only option: {not_filtered_files}\n'
                        'Use "--only which" to show available file filters')

    # now do the filtering
    for category, category_files in mod_files.items():
        mod_files[category] = list(filter(lambda x: x['abbr'] in filter_files, category_files))


def print_available_mod_files(mod_files):
    for category, category_files in mod_files.items():
        print(f'Available files for category "{category}"')
        for file_info in category_files:
            print(f'  {file_info["abbr"]}')


def write_content_to_file(file_info, strategy, only_diff=False):
    # strategy:
    #   update: update only if dest older or not existent
    #   force-update: update, even if dest newer
    #   update-if-non-existent: update only if file does not exists
    #   create: create file only if it does not exist
    #   force-create: create file, even if it exists
    # FIXME (aw): we should have this as an enum

    strategies = ['update', 'force-update', 'update-if-non-existent', 'create', 'force-create']

    file_path = file_info['path']
    file_dir = file_path.parent
    printable_name = file_info['printable_name']

    method = ''

    if only_diff:
        return __show_diff_for(file_info)

    if strategy == 'update':
        if file_path.exists() and file_path.stat().st_mtime > file_info['last_mtime']:
            print(f'Skipping {printable_name} (up-to-date)')
            return
        method = 'Updating'
    elif strategy == 'force-update':
        method = 'Force-updating' if file_path.exists() else 'Creating'
    elif strategy == 'force-create':
        method = 'Overwriting' if file_path.exists() else 'Creating'
    elif strategy == 'update-if-non-existent' or strategy == 'create':
        if file_path.exists():
            print(f'Skipping {printable_name} (use create --force to recreate)')
            return
        method = 'Creating'
    else:
        raise Exception(f'Invalid strategy "{strategy}"\nSupported strategies: {strategies}')

    print(f'{method} file {printable_name}')

    if not file_dir.exists():
        file_dir.mkdir(parents=True, exist_ok=True)

    file_path.write_text(file_info['content'])
