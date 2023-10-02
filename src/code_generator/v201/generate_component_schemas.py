#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

"""This script generates JSON schemas file of OCPP components for v201 based on a modified version of the
    dm_component_vars.csv appendix of the OCPP2.0.1 specification. These component schema files can then
    be used to initialize the device model storage database.
"""

import argparse
from pathlib import Path
import csv

from genson import SchemaBuilder

JSON_SCHEMA_TYPES = ["string", "number", "integer",
                     "object", "array", "boolean", "null"]

STANDARDIZED_COMPONENTS = ["InternalCtrlr", "AlignedDataCtrlr", "AuthCacheCtrlr", "AuthCtrlr", "ClockCtrlr", "CustomizationCtrlr", "DeviceDataCtrlr", "DisplayMessageCtrlr",
                           "ISO15118Ctrlr", "LocalAuthListCtrlr", "MonitoringCtrlr", "OCPPCommCtrlr", "ReservationCtrlr", "SampledDataCtrlr", "SecurityCtrlr", "SmartChargingCtrlr", "TariffCostCtrlr", "TxCtrlr"]


def generate_schemas(csv_path: Path, schema_dir: Path):
    """Generates JSON schema files using the given csv_path. Writes into given schema_dir

    Args:
        csv_path (Path): csv file path
        schema_dir (Path): output file directory
    """
    with open(csv_path) as csv_file:
        table = csv.DictReader(csv_file, delimiter=';')
        table = [e for e in table if e['specific_component'] !=
                 '<generic>' and e['specific_component'] in STANDARDIZED_COMPONENTS]
        schemas = {}
        config_properties = {}
        for variable_entry in table:
            component = variable_entry["specific_component"]

            # add new component to schemas if not yet present
            if component not in schemas:
                schemas[component] = {
                    "name": component,
                    "properties": {},
                    "description": f"Schema for {component}",
                    "required": []
                }
                config_properties[component] = {
                    "type": "object",
                    "$ref": f"{component}.json"
                }

            property_name = variable_entry["unique_variable_name"]

            property_entry = {
                "variable_name": variable_entry["variable_name"],
                "characteristics": {},
                "attributes": [{
                    "type": "Actual"
                }]
            }

            if variable_entry["instance"]:
                property_entry["instance"] = variable_entry["instance"]

            if variable_entry["description"]:
                property_entry["description"] = variable_entry["description"]

            if (bool(int(variable_entry["required"]))):
                schemas[component]['required'].append(property_name)

            if (variable_entry["mutability"]):
                property_entry['attributes'][0]['mutability'] = variable_entry["mutability"]

            if (variable_entry["unit"]):
                property_entry['characteristics']['unit'] = variable_entry["unit"]
            if (variable_entry["min_limit"]):
                property_entry['minimum'] = int(variable_entry["min_limit"])
                property_entry['characteristics']['minLimit'] = int(
                    variable_entry["min_limit"])
            if (variable_entry["max_limit"]):
                property_entry['maximum'] = int(variable_entry["max_limit"])
                property_entry['characteristics']['maxLimit'] = int(
                    variable_entry["max_limit"])

            if (variable_entry["values_list"]):
                property_entry['characteristics']['valuesList'] = variable_entry["values_list"]

            if (variable_entry["supports_monitoring"]):
                property_entry['characteristics']['supportsMonitoring'] = bool(
                    int(variable_entry["supports_monitoring"]))

            if variable_entry['data_type']:
                property_entry['characteristics']['dataType'] = variable_entry["data_type"]
                if variable_entry["data_type"] in JSON_SCHEMA_TYPES:
                    property_entry['type'] = variable_entry["data_type"]
                elif variable_entry["data_type"] == "decimal":
                    property_entry['type'] = 'number'
                elif variable_entry["data_type"] == "dateTime":
                    property_entry['type'] = 'string'
                else:
                    property_entry['type'] = "string"

            if variable_entry['default']:
                property_entry['default'] = variable_entry['default']

            if variable_entry['min_length']:
                property_entry['minLength'] = int(variable_entry['min_length'])

            if variable_entry['max_length']:
                property_entry['maxLength'] = int(variable_entry['max_length'])

            schemas[component]["properties"][property_name] = property_entry

    for key in schemas:
        builder = SchemaBuilder()
        builder.add_schema({"$schema": "http://json-schema.org/draft-07/schema#", "type": "object", "description": schemas[key]["description"],
                           "name":schemas[key]["name"], "required": schemas[key]["required"], "properties": schemas[key]["properties"]})

        file_path = schema_dir / f"{key}.json"
        with open(file_path, "w") as out:
            out.write(builder.to_json(indent=2))
            out.write("\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description="OCPP code generator")
    parser.add_argument("--csv", metavar='CSV',
                        help="Path to dm_components_vars.csv appendix of OCPP2.0.1 spec", required=True)
    parser.add_argument("--out", metavar='OUT',
                        help="Dir in which the generated JSON schemas will be put", required=True)

    args = parser.parse_args()
    csv_path = Path(args.csv).resolve()
    schema_dir = Path(args.out).resolve()

    generate_schemas(csv_path, schema_dir)
