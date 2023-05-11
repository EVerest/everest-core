#!/usr/bin/env -S python3 -tt
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#
"""
author: aw@pionix.de
FIXME (aw): Module documentation.
"""

from . import __version__
from . import helpers
from .type_parsing import TypeParser

from datetime import datetime
from pathlib import Path
import jinja2 as j2
import argparse
import stringcase
from typing import List


# FIXME (aw): remove these global variables

# global variables
everest_dirs: List[Path] = []
work_dir: Path = None

# jinja template environment and global variable
env = j2.Environment(loader=j2.FileSystemLoader(Path(__file__).parent / 'templates'),
                     lstrip_blocks=True, trim_blocks=True, undefined=j2.StrictUndefined,
                     keep_trailing_newline=True)

templates = {}
validators = {}

# Function declarations


def setup_jinja_env():
    env.globals['timestamp'] = datetime.utcnow()
    # FIXME (aw): which repo to use? everest or everest-framework?
    env.globals['git'] = helpers.gather_git_info(work_dir)
    env.filters['snake_case'] = helpers.snake_case
    env.filters['create_dummy_result'] = helpers.create_dummy_result

    templates.update({
        'interface_base': env.get_template('interface-Base.hpp.j2'),
        'interface_exports': env.get_template('interface-Exports.hpp.j2'),
        'interface_impl.hpp': env.get_template('interface-Impl.hpp.j2'),
        'interface_impl.cpp': env.get_template('interface-Impl.cpp.j2'),
        'types.hpp': env.get_template('types.hpp.j2'),
        'module.hpp': env.get_template('module.hpp.j2'),
        'module.cpp': env.get_template('module.cpp.j2'),
        'ld-ev.hpp': env.get_template('ld-ev.hpp.j2'),
        'ld-ev.cpp': env.get_template('ld-ev.cpp.j2'),
        'cmakelists': env.get_template('CMakeLists.txt.j2')
    })


def generate_tmpl_data_for_if(interface, if_def, type_file):
    helpers.parsed_enums.clear()
    helpers.parsed_types.clear()
    helpers.type_headers.clear()
    types = []
    enums = []
    vars = []
    for var, var_info in if_def.get('vars', {}).items():
        (type_info, enum_info) = helpers.extended_build_type_info(var, var_info, type_file)
        if enum_info and type_file:
            enums.append(enum_info)

        vars.append(type_info)

    cmds = []
    for cmd, cmd_info in if_def.get('cmds', {}).items():
        args = []
        for arg, arg_info in cmd_info.get('arguments', {}).items():
            (type_info, enum_info) = helpers.extended_build_type_info(arg, arg_info, type_file)
            if enum_info and type_file:
                enums.append(enum_info)

            args.append(type_info)

        result_type_info = None
        if 'result' in cmd_info:
            result_info = cmd_info['result']

            (result_type_info, enum_info) = helpers.extended_build_type_info('result', result_info, type_file)
            if enum_info and type_file:
                enums.append(enum_info)

        cmds.append({'name': cmd, 'args': args, 'result': result_type_info})

    if type_file:
        for parsed_enum in helpers.parsed_enums:
            enum_info = {
                'name': parsed_enum['name'],
                'description': parsed_enum['description'],
                'enum_type': stringcase.capitalcase(parsed_enum['name']),
                'enum': parsed_enum['enums']
            }
            enums.append(enum_info)

    if type_file:
        for parsed_type in helpers.parsed_types:
            parsed_type['name'] = stringcase.capitalcase(parsed_type['name'])
            if 'properties' in parsed_type:
                for prop in parsed_type['properties']:
                    if 'type_dict' in prop['info']:
                        path = Path('generated/types') / \
                            prop['info']['type_dict']['type_relative_path'].with_suffix('.hpp')
                        helpers.type_headers.add(path.as_posix())

            types.append(parsed_type)

    tmpl_data = {
        'info': {
            'base_class_header': f'generated/interfaces/{interface}/Implementation.hpp',
            'interface': interface,
            'desc': if_def['description'],
            'type_headers': sorted(helpers.type_headers)
        },
        'enums': enums,
        'types': types,
        'vars': vars,
        'cmds': cmds
    }

    return tmpl_data


def generate_tmpl_data_for_module(module, module_def):
    provides = []
    for impl, impl_info in module_def.get('provides', {}).items():
        config = []
        for conf_id, conf_info in impl_info.get('config', {}).items():
            type_info = helpers.build_type_info(conf_id, conf_info['type'])
            config.append(type_info)

        provides.append({
            'id': impl,
            'type': impl_info['interface'],
            'desc': impl_info['description'],
            'config': config,
            'class_name': f'{impl_info["interface"]}Impl',
            'base_class': f'{impl_info["interface"]}ImplBase',
            'base_class_header': f'generated/interfaces/{impl_info["interface"]}/Implementation.hpp'
        })

    requires = []
    for requirement_id, req_info in module_def.get('requires', {}).items():
        # min_connections=1 and max_connections=1 is the default if not provided otherwise (see manifest meta schema)
        is_vector = not (
            ('min_connections' not in req_info or req_info['min_connections'] == 1) and
            ('max_connections' not in req_info or req_info['max_connections'] == 1))
        requires.append({
            'id': requirement_id,
            'is_vector': is_vector,
            'type': req_info['interface'],
            'class_name': f'{req_info["interface"]}Intf',
            'exports_header': f'generated/interfaces/{req_info["interface"]}/Interface.hpp'
        })

    module_config = []
    for conf_id, conf_info in module_def.get('config', {}).items():
        type_info = helpers.build_type_info(conf_id, conf_info['type'])
        module_config.append(type_info)

    tmpl_data = {
        'info': {
            'name': module,
            'class_name': module,  # FIXME (aw): enforce capital case?
            'desc': module_def['description'],
            'module_header': f'{module}.hpp',
            'module_config': module_config,
            'ld_ev_header': 'ld-ev.hpp',
            'enable_external_mqtt': module_def.get('enable_external_mqtt', False),
            'enable_telemetry': module_def.get('enable_telemetry', False)
        },
        'provides': provides,
        'requires': requires,
    }

    return tmpl_data


def construct_impl_file_paths(impl):
    interface = impl['type']
    common_part = f'{impl["id"]}/{interface}'
    return (f'{common_part}Impl.hpp', f'{common_part}Impl.cpp')


def set_impl_specific_path_vars(tmpl_data, output_path):
    """Set cpp_file_rel_path and class_header vars to implementation template data."""
    for impl in tmpl_data['provides']:
        (impl['class_header'], impl['cpp_file_rel_path']) = construct_impl_file_paths(impl)


def generate_module_loader_files(mod, output_dir):
    loader_files = []

    mod_path = work_dir / f'modules/{mod}/manifest.yaml'
    if not mod_path.exists():
        raise Exception(f'Could not find module manifest ({mod_path}')

    mod_def = helpers.load_validated_module_def(mod_path, validators['module'])
    tmpl_data = generate_tmpl_data_for_module(mod, mod_def)

    set_impl_specific_path_vars(tmpl_data, mod_path.parent)

    # ld-ev.hpp
    tmpl_data['info']['hpp_guard'] = 'LD_EV_HPP'

    loader_files.append({
        'filename': 'ld-ev.hpp',
        'path': output_dir / mod / 'ld-ev.hpp',
        'printable_name': f'{mod}/ld-ev.hpp',
        'content': templates['ld-ev.hpp'].render(tmpl_data),
        'last_mtime': mod_path.stat().st_mtime
    })

    # ld-ev.cpp
    loader_files.append({
        'filename': 'ld-ev.cpp',
        'path': output_dir / mod / 'ld-ev.cpp',
        'printable_name': f'{mod}/ld-ev.cpp',
        'content': templates['ld-ev.cpp'].render(tmpl_data),
        'last_mtime': mod_path.stat().st_mtime
    })

    return loader_files


def generate_module_files(mod, update_flag):
    mod_files = {'core': [], 'interfaces': []}
    mod_path = work_dir / f'modules/{mod}/manifest.yaml'
    mod_def = helpers.load_validated_module_def(mod_path, validators['module'])

    tmpl_data = generate_tmpl_data_for_module(mod, mod_def)
    output_path = mod_path.parent
    # FIXME (aw): we might move the following function into generate_tmp_data_for_module
    set_impl_specific_path_vars(tmpl_data, output_path)

    cmakelists_blocks = {
        'version': 'v1',
        'format_str': '# ev@{uuid}:{version}',
        'regex_str': '^(?P<indent>\s*)# ev@(?P<uuid>[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}):(?P<version>.*)$',
        'definitions': {
            'add_general': {
                'id': 'bcc62523-e22b-41d7-ba2f-825b493a3c97',
                'content': '# insert your custom targets and additional config variables here'
            },
            'add_other': {
                'id': 'c55432ab-152c-45a9-9d2e-7281d50c69c3',
                'content': '# insert other things like install cmds etc here'
            }
        }
    }

    impl_hpp_blocks = {
        'version': 'v1',
        'format_str': '// ev@{uuid}:{version}',
        'regex_str': '^(?P<indent>\s*)// ev@(?P<uuid>[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}):(?P<version>.*)$',
        'definitions': {
            'add_headers': {
                'id': '75ac1216-19eb-4182-a85c-820f1fc2c091',
                'content': '// insert your custom include headers here'
            },
            'public_defs': {
                'id': '8ea32d28-373f-4c90-ae5e-b4fcc74e2a61',
                'content': '// insert your public definitions here'
            },
            'protected_defs': {
                'id': 'd2d1847a-7b88-41dd-ad07-92785f06f5c4',
                'content': '// insert your protected definitions here'
            },
            'private_defs': {
                'id': '3370e4dd-95f4-47a9-aaec-ea76f34a66c9',
                'content': '// insert your private definitions here'
            },
            'after_class': {
                'id': '3d7da0ad-02c2-493d-9920-0bbbd56b9876',
                'content': '// insert other definitions here'
            }
        }
    }

    mod_hpp_blocks = {
        'version': 'v1',
        'format_str': '// ev@{uuid}:{version}',
        'regex_str': '^(?P<indent>\s*)// ev@(?P<uuid>[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}):(?P<version>.*)$',
        'definitions': {
            'add_headers': {
                'id': '4bf81b14-a215-475c-a1d3-0a484ae48918',
                'content': '// insert your custom include headers here'
            },
            'public_defs': {
                'id': '1fce4c5e-0ab8-41bb-90f7-14277703d2ac',
                'content': '// insert your public definitions here'
            },
            'protected_defs': {
                'id': '4714b2ab-a24f-4b95-ab81-36439e1478de',
                'content': '// insert your protected definitions here'
            },
            'private_defs': {
                'id': '211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8',
                'content': '// insert your private definitions here'
            },
            'after_class': {
                'id': '087e516b-124c-48df-94fb-109508c7cda9',
                'content': '// insert other definitions here'
            }
        }
    }

    # provided interface implementations (impl cpp & hpp)
    for impl in tmpl_data['provides']:
        interface = impl['type']
        (impl_hpp_file, impl_cpp_file) = construct_impl_file_paths(impl)

        # load template data for interface
        if_def, last_mtime = load_interface_definition(interface)

        if_tmpl_data = generate_tmpl_data_for_if(interface, if_def, False)

        if_tmpl_data['info'].update({
            'hpp_guard': helpers.snake_case(f'{impl["id"]}_{interface}').upper() + '_IMPL_HPP',
            'config': impl['config'],
            'class_name': interface + 'Impl',
            'class_parent': interface + 'ImplBase',
            'module_header': f'../{mod}.hpp',
            'module_class': mod,
            'interface_implementation_id': impl['id']
        })

        if_tmpl_data['info']['blocks'] = helpers.load_tmpl_blocks(
            impl_hpp_blocks, output_path / impl_hpp_file, update_flag)

        # FIXME (aw): time stamp should include parent interfaces modification dates
        mod_files['interfaces'].append({
            'abbr': f'{impl["id"]}.hpp',
            'path': output_path / impl_hpp_file,
            'printable_name': impl_hpp_file,
            'content': templates['interface_impl.hpp'].render(if_tmpl_data),
            'last_mtime': last_mtime
        })

        mod_files['interfaces'].append({
            'abbr': f'{impl["id"]}.cpp',
            'path': output_path / impl_cpp_file,
            'printable_name': impl_cpp_file,
            'content': templates['interface_impl.cpp'].render(if_tmpl_data),
            'last_mtime': last_mtime
        })

    cmakelists_file = output_path / 'CMakeLists.txt'
    tmpl_data['info']['blocks'] = helpers.load_tmpl_blocks(cmakelists_blocks, cmakelists_file, update_flag)
    mod_files['core'].append({
        'abbr': 'cmakelists',
        'path': cmakelists_file,
        'content': templates['cmakelists'].render(tmpl_data),
        'last_mtime': mod_path.stat().st_mtime
    })

    # module.hpp
    tmpl_data['info']['hpp_guard'] = helpers.snake_case(mod).upper() + '_HPP'
    mod_hpp_file = output_path / f'{mod}.hpp'
    tmpl_data['info']['blocks'] = helpers.load_tmpl_blocks(mod_hpp_blocks, mod_hpp_file, update_flag)
    mod_files['core'].append({
        'abbr': 'module.hpp',
        'path': mod_hpp_file,
        'content': templates['module.hpp'].render(tmpl_data),
        'last_mtime': mod_path.stat().st_mtime
    })

    # module.cpp
    mod_cpp_file = output_path / f'{mod}.cpp'
    mod_files['core'].append({
        'abbr': 'module.cpp',
        'path': mod_cpp_file,
        'content': templates['module.cpp'].render(tmpl_data),
        'last_mtime': mod_path.stat().st_mtime
    })

    for file_info in [*mod_files['core'], *mod_files['interfaces']]:
        file_info['printable_name'] = file_info['path'].relative_to(output_path)

    return mod_files


def load_interface_definition(interface):
    if_path = helpers.resolve_everest_dir_path(f'interfaces/{interface}.yaml')

    if_def = helpers.load_validated_interface_def(if_path, validators['interface'])

    if 'vars' not in if_def:
        if_def['vars'] = {}
    if 'cmds' not in if_def:
        if_def['cmds'] = {}

    last_mtime = if_path.stat().st_mtime

    return if_def, last_mtime


def generate_interface_headers(interface, all_interfaces_flag, output_dir):
    if_parts = {'base': None, 'exports': None, 'types': None}

    try:
        if_def, last_mtime = load_interface_definition(interface)
    except Exception as e:
        if not all_interfaces_flag:
            raise
        else:
            # FIXME (aw): should we really silently ignore that?
            print(f'Ignoring interface {interface} with reason: {e}')
            return

    tmpl_data = generate_tmpl_data_for_if(interface, if_def, False)

    output_path = output_dir / interface
    output_path.mkdir(parents=True, exist_ok=True)

    tmpl_data['info']['interface_name'] = f'{interface}'
    tmpl_data['info']['namespace'] = [f'{interface}']

    # generate Base file (providers view)
    tmpl_data['info']['hpp_guard'] = helpers.snake_case(interface).upper() + '_IMPLEMENTATION_HPP'
    tmpl_data['info']['class_name'] = f'{interface}ImplBase'

    base_file = output_path / 'Implementation.hpp'

    if_parts['base'] = {
        'path': base_file,
        'content': templates['interface_base'].render(tmpl_data),
        'last_mtime': last_mtime,
        'printable_name': base_file.relative_to(output_path.parent)
    }

    # generate Exports file (users view)
    tmpl_data['info']['hpp_guard'] = helpers.snake_case(interface).upper() + '_INTERFACE_HPP'
    tmpl_data['info']['class_name'] = f'{interface}Intf'

    exports_file = output_path / 'Interface.hpp'

    if_parts['exports'] = {
        'path': exports_file,
        'content': templates['interface_exports'].render(tmpl_data),
        'last_mtime': last_mtime,
        'printable_name': exports_file.relative_to(output_path.parent)
    }

    # generate Types file
    tmpl_data['info']['hpp_guard'] = helpers.snake_case(interface).upper() + '_TYPES_HPP'

    types_file = output_path / 'Types.hpp'

    if_parts['types'] = {
        'path': types_file,
        'content': templates['types.hpp'].render(tmpl_data),
        'last_mtime': last_mtime,
        'printable_name': types_file.relative_to(output_path.parent)
    }

    return if_parts


def module_create(args):
    create_strategy = 'force-create' if args.force else 'create'

    mod_files = generate_module_files(args.module, False)

    if args.only == 'which':
        helpers.print_available_mod_files(mod_files)
        return
    else:
        try:
            helpers.filter_mod_files(args.only, mod_files)
        except Exception as err:
            print(err)
            return

    for file_info in mod_files['core'] + mod_files['interfaces']:
        if not args.disable_clang_format:
            helpers.clang_format(args.clang_format_file, file_info)

        helpers.write_content_to_file(file_info, create_strategy, args.diff)


def module_update(args):
    # Always generate type info before updating module
    for type_with_namespace in list_types_with_namespace():
        _tmpl_data, _last_mtime = TypeParser.generate_type_info(type_with_namespace, all_types=True)

    primary_update_strategy = 'force-update' if args.force else 'update'
    update_strategy = {'module.cpp': 'update-if-non-existent'}
    for file_name in ['cmakelists', 'module.hpp']:
        update_strategy[file_name] = primary_update_strategy

    # FIXME (aw): refactor out this only handling and rename it properly
    mod_files = generate_module_files(args.module, True)

    if args.only == 'which':
        helpers.print_available_mod_files(mod_files)
        return
    else:
        try:
            helpers.filter_mod_files(args.only, mod_files)
        except Exception as err:
            print(err)
            return

    if not args.disable_clang_format:
        for file_info in mod_files['core'] + mod_files['interfaces']:
            helpers.clang_format(args.clang_format_file, file_info)

    for file_info in mod_files['core']:
        helpers.write_content_to_file(file_info, update_strategy[file_info['abbr']], args.diff)

    for file_info in mod_files['interfaces']:
        if file_info['abbr'].endswith('.hpp'):
            helpers.write_content_to_file(file_info, primary_update_strategy, args.diff)
        else:
            helpers.write_content_to_file(file_info, 'update-if-non-existent', args.diff)


def module_genld(args):
    output_dir = Path(args.output_dir).resolve() if args.output_dir else work_dir / \
        'build/generated/generated/modules'

    loader_files = generate_module_loader_files(args.module, output_dir)

    if not args.disable_clang_format:
        for file_info in loader_files:
            helpers.clang_format(args.clang_format_file, file_info)

    for file_info in loader_files:
        helpers.write_content_to_file(file_info, 'force-update')


def interface_genhdr(args):
    # Always generate type info before generating interfaces
    for type_with_namespace in list_types_with_namespace():
        _tmpl_data, _last_mtime = TypeParser.generate_type_info(type_with_namespace, all_types=True)

    output_dir = Path(args.output_dir).resolve() if args.output_dir else work_dir / \
        'build/generated/include/generated/interfaces'
    primary_update_strategy = 'force-update' if args.force else 'update'

    interfaces = args.interfaces
    all_interfaces = False
    if not interfaces:
        all_interfaces = True
        interfaces = []
        for everest_dir in everest_dirs:
            if_dir = everest_dir / 'interfaces'
            interfaces += [if_path.stem for if_path in if_dir.iterdir() if (if_path.is_file()
                                                                            and if_path.suffix == '.yaml')]

    for interface in interfaces:
        if_parts = generate_interface_headers(interface, all_interfaces, output_dir)

        if not args.disable_clang_format:
            # FIXME (aw): this broken, because in case all_interfaces is true, if_parts might be none for invalid interface files
            helpers.clang_format(args.clang_format_file, if_parts['base'])
            helpers.clang_format(args.clang_format_file, if_parts['exports'])
            helpers.clang_format(args.clang_format_file, if_parts['types'])

        helpers.write_content_to_file(if_parts['base'], primary_update_strategy, args.diff)
        helpers.write_content_to_file(if_parts['exports'], primary_update_strategy, args.diff)
        helpers.write_content_to_file(if_parts['types'], primary_update_strategy, args.diff)


def helpers_genuuids(args):
    if (args.count <= 0):
        raise Exception(f'Invalid number ("{args.count}") of uuids to generate')
    helpers.generate_some_uuids(args.count)


def helpers_yaml2json(args):
    helpers.yaml2json(Path(args.input).resolve(), Path(args.output).resolve())


def helpers_json2yaml(args):
    helpers.json2yaml(Path(args.input).resolve(), Path(args.output).resolve())


def list_types_with_namespace(types=None) -> List:
    if not types:
        types = []
        for everest_dir in everest_dirs:
            types_dir = everest_dir / 'types'
            types += list(types_dir.glob("**/*.yaml"))

    types_with_namespace = []
    for type_path in types:
        relative_path = None
        for everest_dir in everest_dirs:
            types_dir = everest_dir / 'types'
            try:
                relative_path = type_path.relative_to(types_dir).with_suffix('')
                break
            except ValueError:
                pass
        uppercase_path = []
        for part in relative_path.parts:
            uppercase_path.append(stringcase.capitalcase(part))
        namespace = '::'.join(relative_path.parts)
        type_with_namespace = {
            'path': type_path,
            'relative_path': relative_path,
            'namespace': namespace,
            'uppercase_path': uppercase_path,
        }

        types_with_namespace.append(type_with_namespace)

    return types_with_namespace


def types_genhdr(args):
    print("Generating global type headers.")
    output_dir = Path(args.output_dir).resolve() if args.output_dir else work_dir / \
        'build/generated/generated/types'

    primary_update_strategy = 'force-update' if args.force else 'update'

    types = None
    all_types = False
    if 'types' not in args:
        all_types = True
    else:
        types = args.types

    types_with_namespace = list_types_with_namespace(types=types)

    for type_with_namespace in types_with_namespace:
        type_parts = TypeParser.generate_type_headers(type_with_namespace, all_types, output_dir)

        if not args.disable_clang_format:
            helpers.clang_format(args.clang_format_file, type_parts['types'])

        helpers.write_content_to_file(type_parts['types'], primary_update_strategy, args.diff)


def main():
    global validators, everest_dirs, work_dir

    parser = argparse.ArgumentParser(description='Everest command line tool')
    parser.add_argument('--version', action='version', version=f'%(prog)s {__version__}')

    common_parser = argparse.ArgumentParser(add_help=False)

    common_parser.add_argument("--work-dir", "-wd", type=str,
                               help='work directory containing the manifest definitions (default: .)', default=str(Path.cwd()))
    common_parser.add_argument("--everest-dir", "-ed", nargs='*',
                               help='everest directory containing the interface definitions (default: .)', default=[str(Path.cwd())])
    common_parser.add_argument("--schemas-dir", "-sd", type=str,
                               help='everest framework directory containing the schema definitions (default: ../everest-framework/schemas)',
                               default=str(Path.cwd() / '../everest-framework/schemas'))
    common_parser.add_argument("--clang-format-file", type=str, default=str(Path.cwd()),
                               help='Path to the directory, containing the .clang-format file (default: .)')
    common_parser.add_argument("--disable-clang-format", action='store_true', default=False,
                               help="Set this flag to disable clang-format")

    subparsers = parser.add_subparsers(metavar='<command>', help='available commands', required=True)
    parser_mod = subparsers.add_parser('module', aliases=['mod'], help='module related actions')
    parser_if = subparsers.add_parser('interface', aliases=['if'], help='interface related actions')
    parser_hlp = subparsers.add_parser('helpers', aliases=['hlp'], help='helper actions')
    parser_types = subparsers.add_parser('types', aliases=['ty'], help='type related actions')

    mod_actions = parser_mod.add_subparsers(metavar='<action>', help='available actions', required=True)
    mod_create_parser = mod_actions.add_parser('create', aliases=['c'], parents=[
                                               common_parser], help='create module(s)')
    mod_create_parser.add_argument('module', type=str, help='name of the module, that should be created')
    mod_create_parser.add_argument('-f', '--force', action='store_true', help='force overwriting - use with care!')
    mod_create_parser.add_argument('-d', '--diff', '--dry-run', action='store_true',
                                   help='show resulting diff on create or overwrite')
    mod_create_parser.add_argument('--only', type=str,
                                   help='Comma separated filter list of module files, that should be created.  '
                                   'For a list of available files use "--only which".')
    mod_create_parser.set_defaults(action_handler=module_create)

    mod_update_parser = mod_actions.add_parser('update', aliases=['u'], parents=[
                                               common_parser], help='update module(s)')
    mod_update_parser.add_argument('module', type=str, help='name of the module, that should be updated')
    mod_update_parser.add_argument('-f', '--force', action='store_true', help='force overwriting')
    mod_update_parser.add_argument('-d', '--diff', '--dry-run', action='store_true', help='show resulting diff')
    mod_update_parser.add_argument('--only', type=str,
                                   help='Comma separated filter list of module files, that should be updated.  '
                                   'For a list of available files use "--only which".')
    mod_update_parser.set_defaults(action_handler=module_update)

    mod_genld_parser = mod_actions.add_parser(
        'generate-loader', aliases=['gl'], parents=[common_parser], help='generate everest loader')
    mod_genld_parser.add_argument(
        'module', type=str, help='name of the module, for which the loader should be generated')
    mod_genld_parser.add_argument('-o', '--output-dir', type=str, help='Output directory for generated loader '
                                  'files (default: {everest-dir}/build/generated/generated/modules)')
    mod_genld_parser.set_defaults(action_handler=module_genld)

    if_actions = parser_if.add_subparsers(metavar='<action>', help='available actions', required=True)
    if_genhdr_parser = if_actions.add_parser(
        'generate-headers', aliases=['gh'], parents=[common_parser], help='generate headers')
    if_genhdr_parser.add_argument('-f', '--force', action='store_true', help='force overwriting')
    if_genhdr_parser.add_argument('-o', '--output-dir', type=str, help='Output directory for generated interface '
                                  'headers (default: {everest-dir}/build/generated/generated/interfaces)')
    if_genhdr_parser.add_argument('-d', '--diff', '--dry-run', action='store_true', help='show resulting diff')
    if_genhdr_parser.add_argument('interfaces', nargs='*', help='a list of interfaces, for which header files should '
                                  'be generated - if no interface is given, all will be processed and non-processable '
                                  'will be skipped')
    if_genhdr_parser.set_defaults(action_handler=interface_genhdr)

    hlp_actions = parser_hlp.add_subparsers(metavar='<action>', help='available actions', required=True)
    hlp_genuuid_parser = hlp_actions.add_parser('generate-uuids', help='generete uuids')
    hlp_genuuid_parser.add_argument('count', type=int, default=3)
    hlp_genuuid_parser.set_defaults(action_handler=helpers_genuuids)

    hlp_yaml2json_parser = hlp_actions.add_parser('yaml2json', help='convert yaml into json')
    hlp_yaml2json_parser.add_argument('input', type=str, help='path to yaml input file')
    hlp_yaml2json_parser.add_argument('output', type=str, help='path to json output file')
    hlp_yaml2json_parser.set_defaults(action_handler=helpers_yaml2json)

    hlp_json2yaml_parser = hlp_actions.add_parser('json2yaml', help='convert json into yaml')
    hlp_json2yaml_parser.add_argument('input', type=str, help='path to json input file')
    hlp_json2yaml_parser.add_argument('output', type=str, help='path to yaml output file')
    hlp_json2yaml_parser.set_defaults(action_handler=helpers_json2yaml)

    types_actions = parser_types.add_subparsers(metavar='<action>', help='available actions', required=True)
    types_genhdr_parser = types_actions.add_parser(
        'generate-headers', aliases=['gh'], parents=[common_parser], help='generete type headers')
    types_genhdr_parser.add_argument('-f', '--force', action='store_true', help='force overwriting')
    types_genhdr_parser.add_argument('-o', '--output-dir', type=str, help='Output directory for generated type '
                                     'headers (default: {everest-dir}/build/generated/generated/types)')
    types_genhdr_parser.add_argument('-d', '--diff', '--dry-run', action='store_true', help='show resulting diff')
    types_genhdr_parser.add_argument('types', nargs='*', help='a list of types, for which header files should '
                                     'be generated - if no type is given, all will be processed and non-processable '
                                     'will be skipped')
    types_genhdr_parser.set_defaults(action_handler=types_genhdr)

    args = parser.parse_args()

    if 'everest_dir' in args:
        # FIXME (aw): the helper commands do not set everest_dir, work_dir and schema_dirs, but the following common
        #             code has to run for all other commands - we need some better check here than just checking for
        #             'everest_dir' in args!
        for entry in args.everest_dir:
            everest_dir = Path(entry).resolve()
            everest_dirs.append(everest_dir)

        helpers.everest_dirs = everest_dirs

        work_dir = Path(args.work_dir).resolve()

        setup_jinja_env()

        schemas_dir = Path(args.schemas_dir).resolve()
        if not schemas_dir.exists():
            print('The default ("../everest-framework/schemas") xor supplied (via --schemas-dir) schemas directory\n'
                  'doesn\'t exist.\n'
                  f'dir: {schemas_dir}')
            exit(1)

        validators = helpers.load_validators(schemas_dir)

        TypeParser.validators = validators
        TypeParser.templates = templates

    args.action_handler(args)


if __name__ == '__main__':
    try:
        main()
    except helpers.EVerestParsingException as e:
        raise SystemExit(e)
