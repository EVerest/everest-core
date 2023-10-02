
#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

"""This script generates a skeleton of a configuration file for OCPP2.0.1 that can be modified and 
used to insert the variables into the device model storage database using the script 
config/insert_device_model_config.py .
"""

import argparse
from pathlib import Path
import json
from glob import glob


def get_attribute_value(property_entry):
    if "default" in property_entry:
        return property_entry["default"]
    elif property_entry["type"] == "string":
        return ""
    elif property_entry["type"] == "integer":
        return 42
    elif property_entry["type"] == "object":
        return {}
    elif property_entry["type"] == "array":
        return []
    elif property_entry["type"] == "boolean":
        return True
    elif property_entry["type"] == "null":
        return None
    elif property_entry["type"] == "number":
        return 42


def generate_config(schemas: Path, out_file: Path, required_only):
    config = []

    print(schemas)

    component_schema_dirs = glob((schemas / "custom").joinpath(
        "*").as_posix()) + glob((schemas / "standardized").joinpath("*").as_posix())
    component_schema_dirs = [Path(component_schema)
                             for component_schema in component_schema_dirs]

    for file in component_schema_dirs:
        with open(file) as component_schema_file:

            component_schema = json.load(component_schema_file)
            component_config = {
                "name": component_schema["name"],
            }

            if "instance" in component_schema:
                component_config["instance"] = component_schema["instance"]
            if "evse_id" in component_schema:
                component_config["evse_id"] = component_schema["evse_id"]
            if "connector_id" in component_schema:
                component_config["connector_id"] = component_schema["connector_id"]

            component_variables = {}

            for unique_variable_name in component_schema["properties"]:
                property_entry = component_schema["properties"][unique_variable_name]
                config_entry = {
                    "variable_name": property_entry["variable_name"],
                    "attributes": {}
                }

                if "instance" in property_entry:
                    config_entry["instance"] = property_entry["instance"]

                if int(unique_variable_name in component_schema["required"]) or not required_only:
                    for variable_attribute in property_entry["attributes"]:
                        config_entry["attributes"][variable_attribute["type"]] = get_attribute_value(
                            property_entry)
                    component_variables[unique_variable_name] = config_entry

            component_config["variables"] = component_variables
            config.append(component_config)

    with open(out_file, "w") as out:
        out.write(json.dumps(config, indent=4))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description="OCPP code generator")
    parser.add_argument("--schemas", metavar='SCHEMAS',
                        help="Path to OCPP2.0.1 profile schemas", required=True)
    parser.add_argument("--out", metavar='OUT',
                        help="Path to resulting config file", required=True)
    parser.add_argument("--required_only",
                        help="Only required variables will be part of the config", action='store_true')

    args = parser.parse_args()
    schemas = Path(args.schemas).resolve()
    schema_dir = Path(args.out).resolve()
    required_only = args.required_only

    generate_config(schemas, schema_dir, required_only)
