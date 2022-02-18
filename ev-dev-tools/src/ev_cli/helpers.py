# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#
"""
author: aw@pionix.de
FIXME (aw): Module documentation.
"""

from pathlib import Path
import shutil
import subprocess
import re

import json
import jstyleson
import jsonschema

from uuid import uuid4


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
        return '{}' # default initialization for variant
    else:
        return primitive_to_sample_value(json_type)


def gather_git_info(repo):
    git_path = shutil.which('git')
    if git_path is None:
        raise RuntimeError('Could not find git executable - I need it to tag auto generated files')

    run_parms = {'cwd': repo, 'capture_output': True, 'encoding': 'utf-8'}

    if subprocess.run([git_path, 'rev-parse', '--is-inside-work-tree'], **run_parms).returncode != 0:
        raise RuntimeError(f'The directory "{repo}" doesn\'t seem to be a git repository!')

    dirty_flag = False if (subprocess.run([git_path, 'diff', '--quiet'], **run_parms).returncode == 0) else True

    branch = subprocess.run([git_path, 'rev-parse', '--abbrev-ref', 'HEAD'], **run_parms).stdout.rstrip()

    remote_branch_cmd = subprocess.run([git_path, 'rev-parse', '--abbrev-ref',
                                       '--symbolic-full-name', '@{upstream}'], **run_parms)

    remote_branch = remote_branch_cmd.stdout.rstrip() if (remote_branch_cmd.returncode == 0) else None

    commit = subprocess.run([git_path, 'rev-parse', 'HEAD'], **run_parms).stdout.rstrip()

    return {
        'dirty_flag': dirty_flag,
        'branch': branch,
        'remote_branch': remote_branch,
        'commit': commit
    }


cpp_type_map = {
    "null": "boost::blank", # FIXME (aw): check whether boost::blank or boost::none is more appropriate here
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


def load_validators(schema_path):
    # FIXME (aw): we should also patch the schemas like in everest-framework
    validators = {}
    for validator, filename in zip(['interface', 'module', 'config'], ['interface', 'manifest', 'config']):
        try:
            schema = jstyleson.loads((schema_path / f'{filename}.json').read_text())
            jsonschema.Draft7Validator.check_schema(schema)
            validators[validator] = jsonschema.Draft7Validator(schema)
        except OSError as err:
            print(f'Could not open schema file {err.filename}: {err.strerror}')
            exit(1)
        except jsonschema.SchemaError as err:
            print(f'Schema error in schema file {filename}.json')
            raise
        except json.JSONDecodeError as err:
            print(f'Could not parse json from file {filename}.json')
            raise

    return validators


def load_validated_interface_def(if_def_path, validator):
    if_def = {}
    try:
        if_def = jstyleson.loads(if_def_path.read_text())
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
    except json.JSONDecodeError as err:
        raise Exception(f'Could not parse json from interface definition file {if_def_path}') from err

    return if_def


def load_validated_module_def(module_path, validator):
    module_def = {}
    try:
        module_def = jstyleson.loads(module_path.read_text())
        validator.validate(module_def)
    except OSError as err:
        print(f'Could not open module definition file {err.filename}: {err.strerror}')
        exit(1)
    except jsonschema.ValidationError as err:
        print(f'Validation error in module definition file {module_path}')
        raise
    except json.JSONDecodeError as err:
        print(f'Could not parse json from module definition file {module_path}')
        raise

    return module_def


def generate_some_uuids(count):
    for i in range(count):
        print(uuid4())


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
