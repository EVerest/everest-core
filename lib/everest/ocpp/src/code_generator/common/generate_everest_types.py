#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#

"""
This script can be used to generate EVerest yaml type definitions based on OCPP2.0.1 or OCPP2.1 JSON schema files
"""

import re
import html
from typing import List, Optional, Dict
from jinja2 import Environment, FileSystemLoader, select_autoescape
from pathlib import Path
from dataclasses import dataclass
import json
import argparse

from utils import snake_case


@dataclass
class ArrayItem:
    type: str
    ref: Optional[str]
    min_items: Optional[int]
    max_items: Optional[int]


@dataclass
class Property:
    name: str
    description: str
    type: str
    ref: Optional[str]
    item: Optional[ArrayItem]
    required: bool


@dataclass
class DataType:
    name: str
    description: str
    properties: List[Property]


@dataclass
class Enum:
    name: str
    description: str
    values: List[str]

    def __eq__(self, value):
        return self.name == value.name


def setup_jinja_template():
    """
    Sets up and returns the jinja template for everest_type.yaml.jinja
    """
    relative_tmpl_path = Path(__file__).resolve().parent / "templates"
    env = Environment(
        loader=FileSystemLoader(relative_tmpl_path),
        trim_blocks=True,
        lstrip_blocks=True,
        autoescape=select_autoescape(enabled_extensions=("html")),
    )
    env.filters["snake_case"] = snake_case
    return env.get_template("everest_type.yaml.jinja")


def format_yaml_description(description, indent_level=6):
    """
    Format the given description to render well in yaml
    """
    if description is not None:
        description = description.replace("+\r\n", "\n")

        # Remove bold and underscores for formatting
        description = re.sub(r"\*\*?([^*]+)\*\*?", r"\1", description)

        # Remove paragraphs starting with "NOTE:"
        description = re.sub(r"(?m)^NOTE:.*(?:\r?\n)?", "", description)

        # Decode HTML entities
        description = html.unescape(description)

        # Remove references like <<...,...>>
        description = re.sub(r"<<[^>]+>>", "", description)

        # Clean up extra spaces and newlines
        description = re.sub(r"\s*\r?\n\s*", " ", description).strip()

        # Reformat multiline descriptions with proper indentation
        wrapped_lines = re.sub(r"(.{1,80})(?:\s|$)", r"\1\n", description).splitlines()
        indent = " " * indent_level
        formatted_description = "\n".join(
            f"{indent}{line.strip()}" for line in wrapped_lines
        ).strip()
        return formatted_description

    return description


def get_data_type_dependencies(data_type: DataType) -> List[str]:
    """
    Gets a list of all definitions referenced by the given data_type
    """
    dependencies = [_property.ref for _property in data_type.properties if _property.ref]
    for _property in data_type.properties:
        if _property.ref:
            dependencies.append(_property.ref)
        elif _property.item and _property.item.ref:
            dependencies.append(_property.item.ref)
    return dependencies


def sort_data_types(data_types: List[DataType]) -> List[DataType]:
    """
    Sorts the given data_types so that dependencies of types are listed before
    they are used as part of a property of a data_type. This spares forward declaration
    of types.
    """
    sorted_types: List = []
    for data_type in data_types:
        insert_at: int = 0
        depends_on = get_data_type_dependencies(data_type)
        for dep_type in depends_on:
            for i, _type in enumerate(sorted_types):
                # the new one depends on the current
                if _type.name == dep_type:
                    insert_at = max(insert_at, i + 1)
                    break
        sorted_types.insert(insert_at, data_type)
    return sorted_types


def parse_enum(definition_type: Dict) -> Enum:
    """
    Parses Enum of given definition_type.
    """
    values = [
        value.replace(".", "_").replace("-", "_") for value in definition_type["enum"]
    ]
    description = format_yaml_description(definition_type.get("description"))
    return Enum(definition_type.get("javaType"), description, values)


def parse_item(_property: Dict):
    """
    Parses ArrayItem of the given property
    """
    property_item_type = _property.get("items").get("type", "object")
    property_item_ref = None
    if "$ref" in _property.get("items"):
        if _property.get("items").get("$ref").endswith("EnumType"):
            property_item_type = "string"
        property_item_ref = (
            _property.get("items").get("$ref").split("/")[-1].replace("Type", "")
        )
    min_items = _property.get("minItems")
    max_items = _property.get("maxItems")
    return ArrayItem(property_item_type, property_item_ref, min_items, max_items)


def parse_property(
    _property: Dict, property_name: str, definition_type: Dict, definitions: List[Dict]
):
    """
    Parses Property from the given arguments
    """

    property_type = _property.get("type", "object")
    property_ref = None
    if "$ref" in _property:
        property_ref = _property["$ref"].split("/")[-1].replace("Type", "")
    property_required = property_name in definition_type.get("required", [])

    property_description = None
    if "description" in _property:
        property_description = _property.get("description")
    elif property_ref is not None:
        property_description = definitions.get(f"{property_ref}Type").get("description")
    property_item = None
    if property_type == "array":
        property_item = parse_item(_property)
        if property_description is None and property_item.ref is not None:
            property_description = definitions.get(f"{property_item.ref}Type").get(
                "description"
            )
    property_description = format_yaml_description(
        property_description, indent_level=10
    )
    return Property(
        property_name,
        property_description,
        property_type,
        property_ref,
        property_item,
        property_required,
    )


def parse_type(definition_type: Dict, definitions) -> DataType:
    """
    Parses DataType of given definition_type
    """
    properties = []
    for property_name, _property in definition_type.get("properties").items():
        properties.append(
            parse_property(_property, property_name, definition_type, definitions)
        )

    data_type_description = format_yaml_description(definition_type.get("description"))
    return DataType(definition_type.get("javaType"), data_type_description, properties)


def parse_types_and_enums(schema_dir: Path, types_to_generate: List[str]):
    """
    Parses Types and Enums based on the given schema_dir and types_to_generate
    """
    enums = []
    types = []

    schema_files = list(schema_dir.glob("*.json"))
    for schema_file in sorted(schema_files):
        with open(schema_file, "r", encoding="utf-8-sig") as schema_dump:
            schema = json.load(schema_dump)
            definitions = schema.get("definitions")
            if definitions is None:
                continue
            for definition_name, definition_type in definitions.items():
                if "enum" in definition_type and definition_name in types_to_generate:
                    enum = parse_enum(definition_type)
                    if enum not in enums:
                        enums.append(enum)
                elif (
                    "enum" not in definition_type
                    and definition_name in types_to_generate
                ):
                    _type = parse_type(definition_type, definitions)
                    if _type not in types:
                        types.append(_type)

    types = sort_data_types(types)
    return types, enums


def load_schemas_from_directory(schema_dir) -> Dict:
    _schema_files = {}
    schema_files = list(schema_dir.glob("*.json"))
    for schema_file in sorted(schema_files):
        with open(schema_file, "r", encoding="utf-8-sig") as schema_dump:
            schema = json.load(schema_dump)
            if "definitions" in schema:
                _schema_files[schema_file] = schema["definitions"]
    return _schema_files


def resolve_references(definition, to_process, resolved_types):
    if isinstance(definition, dict):
        for key, value in definition.items():
            if key == "$ref":
                ref_type = value.split("/")[-1]
                if ref_type not in resolved_types:
                    to_process.add(ref_type)
            else:
                resolve_references(value, to_process, resolved_types)
    elif isinstance(definition, list):
        for item in definition:
            resolve_references(item, to_process, resolved_types)


def get_required_types(schema_dir, target_types):
    schema_files = load_schemas_from_directory(schema_dir)
    resolved_types = set()
    to_process = set(target_types)

    type_to_file = {}
    for file_name, definitions in schema_files.items():
        for type_name in definitions.keys():
            type_to_file[type_name] = file_name

    while to_process:
        current = to_process.pop()
        if current in resolved_types:
            continue

        resolved_types.add(current)
        file_name = type_to_file.get(current)
        if not file_name:
            raise ValueError(f"Type {current} not found in any schema file.")

        definition = schema_files[file_name].get(current, {})
        resolve_references(definition, to_process, resolved_types)

    return resolved_types


def generate_everest_types(
    schema_dir: Path, out_file: Path, types_to_generate: List[str]
):

    types, enums = parse_types_and_enums(schema_dir, types_to_generate)

    with open(out_file, "w") as out:
        out.write(
            setup_jinja_template().render(
                {
                    "enums": enums,
                    "types": types,
                }
            )
        )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter,
        description="Code generator for EVerest types from OCPP JSON schemas",
    )
    parser.add_argument(
        "--schemas",
        metavar="SCHEMAS",
        help="Directory in which the OCPP schemas reside",
        required=True,
    )
    parser.add_argument(
        "--out",
        metavar="OUT",
        help="Dir in which the generated EVerest YAML types will be put",
        required=True,
    )
    parser.add_argument(
        "--types",
        metavar="OUT",
        help="Specify comma seperated list of OCPP Types that should be generated in YAML",
        required=False,
    )

    args = parser.parse_args()

    schema_dir = Path(args.schemas).resolve()
    out_file = Path(args.out).resolve()
    types_to_generate = []
    if args.types:
        types_to_generate = args.types.split(",")
    types_to_generate = get_required_types(schema_dir, types_to_generate)

    generate_everest_types(schema_dir, out_file, types_to_generate)
