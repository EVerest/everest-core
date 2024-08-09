#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#
"""OCPP JSON schema to cpp converter."""
from jinja2 import Environment, FileSystemLoader, select_autoescape
import json
import stringcase
from typing import Dict, List, Tuple
import keyword
from datetime import datetime
import argparse
from pathlib import Path
import subprocess


def snake_case(word: str) -> str:
    """Convert capital case to snake case.
    Only alphanumerical characters are allowed. Only inserts camelcase
    between a consecutive lower and upper alphabetical character and
    lowers first letter.
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
        if (last_char.islower() and last_char.isalpha() and
                cur_char.isupper() and cur_char.isalpha):
            out += '_'
        if not cur_char.isalnum():
            out += '_'
        else:
            out += cur_char.lower()

    return out


def remove_last(input_string: str, search: str) -> str:
    """Removes the last occurence of search"""
    if input_string.endswith(search):
        return input_string[:input_string.rfind(search)]
    return input_string


def uses_optional(types):
    for t in types:
        for property in t['properties']:
            if not property['required']:
                return True
    return False


def needs_enums(types):
    for t in types:
        for property in t['properties']:
            if property['enum']:
                return True
    return False


def needs_types(types):
    type_list = ['CiString<6>', 'CiString<8>', 'CiString<16>', 'CiString<20>', 'CiString<25>', 'CiString<32>', 'CiString<36>',
                 'CiString<50>', 'CiString<64>', 'CiString<255>', 'CiString<500>', 'CiString<1000>', 'CiString<2500>', 'CiString<5600>', 'CiString<7500>', 'DateTime']
    for t in types:
        for property in t['properties']:
            if property['type'] in type_list:
                return True
    return False


# jinja template environment and global variable
relative_tmpl_path = Path(__file__).resolve().parent / "templates"
env = Environment(
    loader=FileSystemLoader(relative_tmpl_path),
    trim_blocks=True,
    autoescape=select_autoescape(
        enabled_extensions=('html'))
)
env.filters['snake_case'] = snake_case
env.filters['remove_last'] = remove_last
env.globals['timestamp'] = datetime.utcnow
env.globals['year'] = datetime.utcnow().year
message_hpp_template = env.get_template('message.hpp.jinja')
message_cpp_template = env.get_template('message.cpp.jinja')
messages_cmakelists_txt_template = env.get_template(
    'messages.cmakelists.txt.jinja')
enums_hpp_template = env.get_template('ocpp_enums.hpp.jinja')
enums_cpp_template = env.get_template('ocpp_enums.cpp.jinja')
ocpp_types_hpp_template = env.get_template('ocpp_types.hpp.jinja')
ocpp_types_cpp_template = env.get_template('ocpp_types.cpp.jinja')

# global variables, should go into a class
parsed_types: List = []
parsed_types_unique: List = []
parsed_enums: List = []
parsed_enums_unique: List = []
current_defs: Dict = {}
unique_types = set()

format_types = dict()
format_types['date-time'] = 'ocpp::DateTime'
format_types['uri'] = 'std::string'  # FIXME(kai): add proper URI type

enum_types = dict()

enum_types['IdTagInfo'] = dict()
enum_types['IdTagInfo']['status'] = 'AuthorizationStatus'
enum_types['BootNotificationResponse'] = dict()
enum_types['BootNotificationResponse']['status'] = 'RegistrationStatus'
enum_types['CancelReservationResponse'] = dict()
enum_types['CancelReservationResponse']['status'] = 'CancelReservationStatus'
enum_types['ChangeAvailabilityRequest'] = dict()
enum_types['ChangeAvailabilityRequest']['type'] = 'AvailabilityType'
enum_types['ChangeAvailabilityResponse'] = dict()
enum_types['ChangeAvailabilityResponse']['status'] = 'AvailabilityStatus'
enum_types['ChangeConfigurationResponse'] = dict()
enum_types['ChangeConfigurationResponse']['status'] = 'ConfigurationStatus'
enum_types['ClearCacheResponse'] = dict()
enum_types['ClearCacheResponse']['status'] = 'ClearCacheStatus'
enum_types['ClearChargingProfileRequest'] = dict()
enum_types['ClearChargingProfileRequest']['chargingProfilePurpose'] = 'ChargingProfilePurposeType'
enum_types['ClearChargingProfileResponse'] = dict()
enum_types['ClearChargingProfileResponse']['status'] = 'ClearChargingProfileStatus'
enum_types['DataTransferResponse'] = dict()
enum_types['DataTransferResponse']['status'] = 'DataTransferStatus'
enum_types['DiagnosticsStatusNotificationRequest'] = dict()
enum_types['DiagnosticsStatusNotificationRequest']['status'] = 'DiagnosticsStatus'
enum_types['FirmwareStatusNotificationRequest'] = dict()
enum_types['FirmwareStatusNotificationRequest']['status'] = 'FirmwareStatus'
enum_types['GetCompositeScheduleRequest'] = dict()
enum_types['GetCompositeScheduleRequest']['chargingRateUnit'] = 'ChargingRateUnit'
enum_types['GetCompositeScheduleResponse'] = dict()
enum_types['GetCompositeScheduleResponse']['status'] = 'GetCompositeScheduleStatus'
enum_types['SampledValue'] = dict()
enum_types['SampledValue']['context'] = 'ReadingContext'
enum_types['SampledValue']['format'] = 'ValueFormat'
enum_types['SampledValue']['measurand'] = 'Measurand'
enum_types['SampledValue']['phase'] = 'Phase'
enum_types['SampledValue']['location'] = 'Location'
enum_types['SampledValue']['unit'] = 'UnitOfMeasure'
enum_types['ChargingProfile'] = dict()
enum_types['ChargingProfile']['chargingProfilePurpose'] = 'ChargingProfilePurposeType'
enum_types['ChargingProfile']['chargingProfileKind'] = 'ChargingProfileKindType'
enum_types['ChargingProfile']['recurrencyKind'] = 'RecurrencyKindType'
enum_types['RemoteStartTransactionResponse'] = dict()
enum_types['RemoteStartTransactionResponse']['status'] = 'RemoteStartStopStatus'
enum_types['RemoteStopTransactionResponse'] = dict()
enum_types['RemoteStopTransactionResponse']['status'] = 'RemoteStartStopStatus'
enum_types['ReserveNowResponse'] = dict()
enum_types['ReserveNowResponse']['status'] = 'ReservationStatus'
enum_types['ResetRequest'] = dict()
enum_types['ResetRequest']['type'] = 'ResetType'
enum_types['ResetResponse'] = dict()
enum_types['ResetResponse']['status'] = 'ResetStatus'
enum_types['SendLocalListRequest'] = dict()
enum_types['SendLocalListRequest']['updateType'] = 'UpdateType'
enum_types['SendLocalListResponse'] = dict()
enum_types['SendLocalListResponse']['status'] = 'UpdateStatus'
enum_types['CsChargingProfiles'] = dict()
enum_types['CsChargingProfiles']['chargingProfilePurpose'] = 'ChargingProfilePurposeType'
enum_types['CsChargingProfiles']['chargingProfileKind'] = 'ChargingProfileKindType'
enum_types['CsChargingProfiles']['recurrencyKind'] = 'RecurrencyKindType'
enum_types['SetChargingProfileRequest'] = dict()
enum_types['SetChargingProfileRequest']['CsChargingProfiles'] = 'ChargingProfile'
enum_types['SetChargingProfileResponse'] = dict()
enum_types['SetChargingProfileResponse']['status'] = 'ChargingProfileStatus'
enum_types['StatusNotificationRequest'] = dict()
enum_types['StatusNotificationRequest']['errorCode'] = 'ChargePointErrorCode'
enum_types['StatusNotificationRequest']['status'] = 'ChargePointStatus'
enum_types['StopTransactionRequest'] = dict()
enum_types['StopTransactionRequest']['reason'] = 'Reason'
enum_types['TriggerMessageRequest'] = dict()
enum_types['TriggerMessageRequest']['requestedMessage'] = 'MessageTrigger'
enum_types['TriggerMessageResponse'] = dict()
enum_types['TriggerMessageResponse']['status'] = 'TriggerMessageStatus'
enum_types['UnlockConnectorResponse'] = dict()
enum_types['UnlockConnectorResponse']['status'] = 'UnlockStatus'
enum_types['ExtendedTriggerMessageRequest'] = dict()
enum_types['ExtendedTriggerMessageRequest']['requestedMessage'] = 'MessageTriggerEnumType'
enum_types['ExtendedTriggerMessageResponse'] = dict()
enum_types['ExtendedTriggerMessageResponse']['status'] = 'TriggerMessageStatusEnumType'
enum_types['SecurityEventNotificationRequest'] = dict()
enum_types['SecurityEventNotificationRequest']['type'] = 'SecurityEvent'


def object_exists(name: str) -> bool:
    """Check if an object (i.e. dataclass) already exists."""
    for el in parsed_types:
        if el['name'] == name:
            return True

    return False


def add_enum_type(name: str, enums: Tuple[str]):
    """Add special class for enum types."""
    for el in parsed_enums:
        if el['name'] == name:
            raise Exception('Warning: enum ' + name + ' already exists')
    print("Adding enum type: %s" % name)
    add = True
    for el in parsed_enums_unique:
        if el['name'] == name and el['enums'] == enums:
            print("non-unique but same enum detected: %s:%s" % (name, enums))
            add = False
    if add:
        parsed_enums.append({
            'name': name,
            'enums': enums
        })
    parsed_enums_unique.append({
        'name': name,
        'enums': enums
    })


def parse_property(prop_name: str, prop: Dict, depends_on: List[str], ob_name=None) -> Tuple[str, bool]:
    """Determine type of property and proceed with it.
    In case it is a $ref, look it up in the current_defs dict and parse
    it again.
    Currently, the following property types are supported:
    - string (and enum as a special case)
    - integer
    - number
    - boolean
    - array
    - object (will be parsed recursivly)
    """

    prop_type = None
    is_enum = False
    if 'type' not in prop:
        if '$ref' in prop:
            prop_type = prop['$ref'].split('/')[-1]
            if prop_type not in current_defs:
                raise Exception('$ref: ' + prop['$ref'] + ' not defined!')
            def_prop = current_defs[prop_type]
            prop_type = def_prop.get('javaType', prop_type)

            return parse_property(prop_type, def_prop, depends_on)
        else:
            # if not defined, propably any
            print(f"{prop_name} has no type defined")
            return ('json', False)
    if prop['type'] == 'string':
        if 'enum' in prop:
            prop_type = stringcase.capitalcase(prop_name)
            if ob_name is not None and ob_name in enum_types:
                print("%s changing type of enum %s from %s to '%s'" %
                      (ob_name, prop_name, prop_type, enum_types[ob_name][prop_name]))
                prop_type = enum_types[ob_name][prop_name]
            print("obname: %s, prop_type: %s" % (ob_name, prop_type))
            add_enum_type(prop_type, prop['enum'])
            is_enum = True
        else:
            if 'maxLength' in prop:
                prop_type = 'CiString<{}>'.format(prop['maxLength'])
            else:
                if 'format' in prop:
                    if prop['format'] in format_types:
                        prop_type = format_types[prop['format']]
                    else:
                        prop_type = 'TODO: std::string with format: {}'.format(
                            prop['format'])
                else:
                    prop_type = 'std::string'
    elif prop['type'] == 'integer':
        prop_type = 'int32_t'
    elif prop['type'] == 'number':
        prop_type = 'float'
    elif prop['type'] == 'boolean':
        prop_type = 'bool'
    elif prop['type'] == 'array':
        prop_type = 'std::vector<' + \
            parse_property(prop_name, prop['items'], depends_on)[0] + '>'
    elif prop['type'] == 'object':
        prop_type = stringcase.capitalcase(prop_name)
        print("!!! prop_type: %s" % prop_type)
        if prop_type == 'ConfigurationKey':
            prop_type = 'KeyValue'
            print("Changed prop type from ConfigurationKey to KeyValue according to spec")
        if prop_type == 'CsChargingProfiles':
            prop_type = 'ChargingProfile'
            print(
                "Changed prop type from CsChargingProfiles to ChargingProfile according to spec")
        if prop_type == 'CertificateHashData':
            prop_type = 'CertificateHashDataType'
            print(
                "Changed prop type from CertificateHashData to CertificateHashDataType according to spec")
        depends_on.append(prop_type)
        if not object_exists(prop_type):
            parse_object(prop_type, prop)
    else:
        raise Exception('Unknown type: ' + prop['type'])

    return (prop_type, is_enum)


def parse_object(ob_name: str, json_schema: Dict):
    """Parse a JSON schema object.
    Iterates over the properties of this object, parses their type
    and puts these information into the global dict parsed_types.
    """

    ob_dict = {'name': ob_name, 'properties': [], 'depends_on': []}
    parsed_types.insert(0, ob_dict)
    if not 'properties' in json_schema:
        return
    for prop_name, prop in json_schema['properties'].items():
        if not prop_name.isidentifier() or keyword.iskeyword(prop_name):
            raise Exception(prop_name + ' can\'t be used as an identifier!')
        prop_type, is_enum = parse_property(
            prop_name, prop, ob_dict['depends_on'], ob_name)
        if not is_enum:
            is_enum = 'enum' in prop
        for parsed_enum in parsed_enums:
            if parsed_enum['name'] == prop_type:
                is_enum = True
                break
        ob_dict['properties'].append({
            'name': prop_name,
            'json_name': prop_name,
            'type': prop_type,
            'enum': is_enum,
            'required': prop_name in json_schema.get('required', {})
        })

    ob_dict['properties'].sort(key=lambda x: x.get('required'), reverse=True)


def parse_schemas(version: str, schema_dir: Path = Path('schemas/json/'),
                  generated_dir: Path = Path('generated/')):
    """Main entry for parsing OCPP json schema files.
    Looks up the corresponding Request/Response JSON schema files for
    each action.  Parses each schema, generates out the corresponding
    python source code, containing the objects in the schema as
    dataclasses, and generate validation scripts.
    """
    schemas = dict()
    schema_files = list(schema_dir.glob('*.json'))
    for schema_file in sorted(schema_files):
        print(f"Schema file: {schema_file}")
        with open(schema_file, 'r') as schema_dump:
            schema = json.load(schema_dump)
            stripped_fn = schema_file.stem
            action = ''
            request = False
            if stripped_fn.endswith('Request'):
                request = True
                action = stripped_fn[0:-7]
            elif stripped_fn.endswith('Response'):
                action = stripped_fn[0:-8]
            elif Path(schema_dir,  stripped_fn + 'Response.json').exists():
                request = True
                action = stripped_fn
            else:
                raise Exception('Invalid schema file', schema_file)

            schemas.setdefault(action, {}).update(
                {'req' if request else 'res': schema})

    # check if for every action, the request and response exists
    for key, value in schemas.items():
        if 'req' not in value or 'res' not in value:
            raise Exception(
                'Either response or request is missing for action: ' + key)

    generated_header_dir = generated_dir / 'include' / 'ocpp' / version
    generated_source_dir = generated_dir / 'lib' / 'ocpp' / version

    if not generated_header_dir.exists():
        generated_header_dir.mkdir(parents=True)
    if not generated_source_dir.exists():
        generated_source_dir.mkdir(parents=True)

    enums_hpp_fn = Path(generated_header_dir, 'ocpp_enums.hpp')
    enums_cpp_fn = Path(generated_source_dir, 'ocpp_enums.cpp')
    ocpp_types_hpp_fn = Path(generated_header_dir, 'ocpp_types.hpp')
    ocpp_types_cpp_fn = Path(generated_source_dir, 'ocpp_types.cpp')
    messages_header_dir = generated_header_dir / 'messages'
    messages_source_dir = generated_source_dir / 'messages'
    messages_cmakelists_txt_fn = Path(messages_source_dir, 'CMakeLists.txt')
    message_files = []
    first = True
    for action, type_of_action in schemas.items():
        message_files.append(action)
        if not messages_header_dir.exists():
            messages_header_dir.mkdir(parents=True)
        if not messages_source_dir.exists():
            messages_source_dir.mkdir(parents=True)
        writemode = dict()
        writemode['req'] = 'w'
        writemode['res'] = 'a+'

        message_uses_optional = False
        message_needs_enums = False
        message_needs_types = False

        for (type_name, type_key) in (('Request', 'req'), ('Response', 'res')):
            parsed_types.clear()
            parsed_enums.clear()

            # FIXME!
            current_defs.clear()
            current_defs.update(
                type_of_action[type_key].get('definitions', {}))

            action_class_name: str = action + type_name

            root_schema = type_of_action[type_key]

            parse_object(action_class_name, root_schema)

            for e in parsed_enums:
                parsed_enums_unique.remove(e)

            # sort types, so no foward declaration is necessary

            sorted_types: List = []
            for class_type in parsed_types:
                insert_at: int = 0
                for dep_class_type in class_type['depends_on']:
                    for i in range(len(sorted_types)):
                        # the new one depends on the current
                        if sorted_types[i]['name'] == dep_class_type:
                            insert_at = max(insert_at, i + 1)
                            break

                sorted_types.insert(insert_at, class_type)

            if not message_uses_optional:
                message_uses_optional = uses_optional(sorted_types)
            if not message_needs_enums:
                message_needs_enums = needs_enums(sorted_types)
            if not message_needs_types:
                message_needs_types = needs_types(sorted_types)

        for (type_name, type_key) in (('Request', 'req'), ('Response', 'res')):
            parsed_types.clear()
            parsed_enums.clear()

            # FIXME!
            current_defs.clear()
            current_defs.update(
                type_of_action[type_key].get('definitions', {}))

            action_class_name: str = action + type_name

            root_schema = type_of_action[type_key]

            parse_object(action_class_name, root_schema)

            # sort types, so no foward declaration is necessary

            sorted_types: List = []
            for class_type in parsed_types:
                insert_at: int = 0
                for dep_class_type in class_type['depends_on']:
                    for i in range(len(sorted_types)):
                        # the new one depends on the current
                        if sorted_types[i]['name'] == dep_class_type:
                            insert_at = max(insert_at, i + 1)
                            break

                sorted_types.insert(insert_at, class_type)

            generated_class_hpp_fn = Path(messages_header_dir, action + '.hpp')
            with open(generated_class_hpp_fn, writemode[type_key]) as out:
                out.write(message_hpp_template.render({
                    'types': sorted_types,
                    'namespace': version_path,
                    'enum_types': parsed_enums,
                    'uses_optional': message_uses_optional,
                    'needs_enums': message_needs_enums,
                    'needs_types': message_needs_types,
                    'action': {
                        'name': action,
                        'class_name': action_class_name,
                        'type_key': type_key,
                        'is_request': (type_name == 'Request')
                    }
                }))
            generated_class_cpp_fn = Path(messages_source_dir, action + '.cpp')
            with open(generated_class_cpp_fn, writemode[type_key]) as out:
                out.write(message_cpp_template.render({
                    'types': sorted_types,
                    'namespace': version_path,
                    'enum_types': parsed_enums,
                    'uses_optional': message_uses_optional,
                    'needs_enums': message_needs_enums,
                    'needs_types': message_needs_types,
                    'action': {
                        'name': action,
                        'class_name': action_class_name,
                        'type_key': type_key,
                        'is_request': (type_name == 'Request')
                    }
                }))
            with open(enums_hpp_fn, 'w' if first else 'a+') as out:
                out.write(enums_hpp_template.render({
                    'enum_types': parsed_enums,
                    'namespace': version_path,
                    'first': first,
                    'action': {
                        'name': action,
                        'class_name': action_class_name,
                    }
                }))
            with open(enums_cpp_fn, 'w' if first else 'a+') as out:
                out.write(enums_cpp_template.render({
                    'enum_types': parsed_enums,
                    'namespace': version_path,
                    'first': first,
                    'action': {
                        'name': action,
                        'class_name': action_class_name,
                    }
                }))
            parsed_types_ = []
            for parsed_type in sorted_types:
                if parsed_type not in parsed_types_unique:
                    parsed_types_unique.append(parsed_type)
                    if parsed_type['name'] not in unique_types:
                        parsed_types_.append(parsed_type)
                        unique_types.add(parsed_type['name'])
            with open(ocpp_types_hpp_fn, 'w' if first else 'a+') as out:
                out.write(ocpp_types_hpp_template.render({
                    'parsed_types': parsed_types_,
                    'namespace': version_path,
                    'first': first,
                    'action': {
                        'name': action,
                        'class_name': action_class_name,
                    }
                }))
            with open(ocpp_types_cpp_fn, 'w' if first else 'a+') as out:
                out.write(ocpp_types_cpp_template.render({
                    'parsed_types': parsed_types_,
                    'namespace': version_path,
                    'first': first,
                    'action': {
                        'name': action,
                        'class_name': action_class_name,
                    }
                }))
            first = False

    with open(messages_cmakelists_txt_fn, 'w') as out:
        out.write(messages_cmakelists_txt_template.render({
            'messages': sorted(message_files)
        }))
    with open(enums_hpp_fn, 'a+') as out:
        out.write(enums_hpp_template.render({
            'last': True,
            'namespace': version_path
        }))
    with open(enums_cpp_fn, 'a+') as out:
        out.write(enums_cpp_template.render({
            'last': True,
            'namespace': version_path
        }))
    with open(ocpp_types_hpp_fn, 'a+') as out:
        out.write(ocpp_types_hpp_template.render({
            'last': True,
            'namespace': version_path
        }))
    with open(ocpp_types_cpp_fn, 'a+') as out:
        out.write(ocpp_types_cpp_template.render({
            'last': True,
            'namespace': version_path
        }))

    # clang-format generated files
    subprocess.run(["sh", "-c", "find {} -regex '.*\\.\\(cpp\\|hpp\\)' -exec clang-format -style=file -i {{}} \\;".format(
        messages_header_dir)], cwd=messages_header_dir)
    subprocess.run(["sh", "-c", "find {} -regex '.*\\.\\(cpp\\|hpp\\)' -exec clang-format -style=file -i {{}} \\;".format(
        messages_source_dir)], cwd=messages_source_dir)
    subprocess.run(["clang-format", "-style=file",  "-i",
                   enums_hpp_fn, ocpp_types_hpp_fn], cwd=generated_header_dir)
    subprocess.run(["clang-format", "-style=file",  "-i",
                   enums_cpp_fn, ocpp_types_cpp_fn], cwd=generated_source_dir)
    subprocess.run(["clang-format", "-style=file",  "-i",
                   enums_cpp_fn], cwd=generated_source_dir)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description="OCPP code generator")
    parser.add_argument("--schemas", metavar='SCHEMAS',
                        help="Directory in which the OCPP 1.6 schemas reside", required=True)
    parser.add_argument("--out", metavar='OUT',
                        help="Dir in which the generated code will be put", required=True)
    parser.add_argument("--version", metavar='VERSION',
                        help="Version of OCPP [1.6 or 2.0.1]", required=True)

    args = parser.parse_args()
    version = args.version

    if version == '1.6' or version == '16' or version == 'v16':
        version_path = 'v16'
    elif version == '2.0.1' or version == '201' or version == 'v201':
        version_path = 'v201'
    else:
        raise ValueError(f"Version {version} not a valid ocpp version")

    schema_dir = Path(args.schemas).resolve()
    generated_dir = Path(args.out).resolve()

    parse_schemas(version=version_path, schema_dir=schema_dir,
                  generated_dir=generated_dir)
