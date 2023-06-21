#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

"""This script generates the ctrlr_component_variables cpp and hpp file of this lib for v201. 
These files provide access to ComponentVariables with a role standardized in the OCPP2.0.1 specification.
"""

from pathlib import Path
from jinja2 import Environment, FileSystemLoader, select_autoescape
import csv
import argparse

JSON_SCHEMA_TYPES = ["string", "number", "integer",
                     "object", "array", "boolean", "null"]

relative_tmpl_path = Path(__file__).resolve().parent / "templates"
env = Environment(
    loader=FileSystemLoader(relative_tmpl_path),
    trim_blocks=True,
    autoescape=select_autoescape(
        enabled_extensions=('html'))
)

template_hpp = env.get_template('controller_component_variables.hpp.jinja')
template_cpp = env.get_template('controller_component_variables.cpp.jinja')


def generate_ctrlr_component_vars(csv_path: Path, libocpp_dir: Path):
    """Generates JSON schema files using the given csv_path. Writes into given schema_dir

    Args:
        csv_path (Path): csv file path
        schema_dir (Path): output file directory
    """

    hpp_file = libocpp_dir / "include/ocpp/v201/ctrlr_component_variables.hpp"
    cpp_file = libocpp_dir / "lib/ocpp/v201/ctrlr_component_variables.cpp"

    with open(csv_path) as csv_file:
        table = csv.DictReader(csv_file, delimiter=';')
        table = [e for e in table if e['specific_component'] != '<generic>']
        variables = []
        components = []

        for variable_entry in table:
            component = variable_entry["specific_component"]

            if variable_entry["standardized"] == "1" or component == "InternalCtrlr":
                if component not in components:
                    components.append(component)

                variables.append({
                    "component_name": component,
                    "unique_variable_name": variable_entry["unique_variable_name"],
                    "variable_name": variable_entry["variable_name"],
                    "instance": variable_entry["instance"],
                    "description": f"Schema for {component}",
                    "required": []
                })

    with open(hpp_file, 'w') as f:
        f.write(template_hpp.render({
            "components": components,
            "variables": variables
        }))

    with open(cpp_file, 'w') as f:
        f.write(template_cpp.render({
            "components": components,
            "variables": variables
        }))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description="OCPP code generator")
    parser.add_argument("--csv", metavar='CSV',
                        help="Path to dm_components_vars.csv appendix of OCPP2.0.1 spec", required=True)
    parser.add_argument("--out", metavar='OUT',
                        help="Dir to libocpp", required=True)

    args = parser.parse_args()
    csv_path = Path(args.csv).resolve()
    libocpp_dir = Path(args.out).resolve()

    generate_ctrlr_component_vars(csv_path, libocpp_dir)
