#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: andreas.heinrich@pionix.de
This script processes a template file with Jinja2 and YAML data.
"""


import argparse
import jinja2
import yaml
from pathlib import Path


def rst_indent(input):
    lines = input.splitlines()
    lines = [f"| {line}\r\n" for line in lines]
    return "".join(lines)


def make_rst_ref(input):
    output = input.replace("/", "")
    output = output.replace("#", "-")
    return output


def literal_rst_filter(value):
    """Wraps a string in double backticks to treat it as a literal in RST."""
    str_value = str(value)
    has_trailing_whitespace = (str_value != str_value.rstrip())
    cleaned_value = str_value.rstrip()
    parts = cleaned_value.split(':', 1)
    if len(parts) == 2 and parts[0] == "pattern":
        formatted_value = f"{parts[0]}:``{parts[1].lstrip()}``"
    else:
        formatted_value = cleaned_value
    if has_trailing_whitespace:
        return formatted_value + '\r\n'
    else:
        return formatted_value


def main():
    parser = argparse.ArgumentParser(description='Processes a template file with Jinja2 and YAML data.')
    parser.add_argument(
        '--template-dir',
        type=Path,
        dest='template_dir',
        action='store',
        required=True,
        help='Directory containing the Jinja2 template files'
    )
    parser.add_argument(
        '--template-file',
        type=Path,
        dest='template_file',
        action='store',
        required=True,
        help='Jinja2 template file to process'
    )
    parser.add_argument(
        '--name',
        type=str,
        dest='name',
        action='store',
        required=True,
        help='Name to be used in the template rendering'
    )
    parser.add_argument(
        '--data-file',
        type=Path,
        dest='data_file',
        action='store',
        required=True,
        help='YAML file containing data for the template'
    )
    parser.add_argument(
        '--module-handwritten-doc',
        type=Path,
        dest='module_handwritten_doc',
        action='store',
        help='Path to the handwritten module documentation if it exists'
    )
    parser.add_argument(
        '--errors-yaml-path',
        type=Path,
        dest='errors_path',
        action='store',
        help='Path to the error definition yaml files'
    )
    parser.add_argument(
        '--target-file',
        type=Path,
        dest='target_file',
        action='store',
        required=True,
        help='Output file for the processed template'
    )
    args = parser.parse_args()

    if not args.template_dir.is_absolute():
        raise ValueError("Template directory path must be absolute")
    if not args.template_dir.exists():
        raise ValueError("Template directory does not exist")
    if not args.template_dir.is_dir():
        raise ValueError("Template directory path is not a directory")

    if not args.template_file.is_absolute():
        raise ValueError("Template file path must be absolute")
    if not args.template_file.exists():
        raise ValueError("Template file does not exist")
    if not args.template_file.is_file():
        raise ValueError("Template file path is not a file")
    if not args.template_file.is_relative_to(args.template_dir):
        raise ValueError("Template file path is not relative to template directory")

    if not args.data_file.is_absolute():
        raise ValueError("Data file path must be absolute")
    if not args.data_file.exists():
        raise ValueError("Data file does not exist")
    if not args.data_file.is_file():
        raise ValueError("Data file path is not a file")
    if args.data_file.suffix not in ['.yml', '.yaml']:
        raise ValueError("Data file must have a .yml or .yaml extension")

    if not args.target_file.is_absolute():
        raise ValueError("Target file path must be absolute")
    if args.target_file.suffix != '.rst':
        raise ValueError("Target file must have a .rst extension")

    if args.errors_path:
        if not args.errors_path.is_absolute():
            raise ValueError("Errors yaml directory path must be absolute")
        if not args.errors_path.exists():
            raise ValueError(f"Errors yaml directory '{args.errors_path}' does not exist")
        if not args.errors_path.is_dir():
            raise ValueError("Errors yaml directory path is not a directory")

    if not args.target_file.parent.exists():
        args.target_file.parent.mkdir(parents=True, exist_ok=True)

    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(args.template_dir),
        trim_blocks=True,
        lstrip_blocks=True
    )
    env.filters['rst_indent'] = rst_indent
    env.filters['make_rst_ref'] = make_rst_ref
    env.filters['literal_rst'] = literal_rst_filter

    template_file_name = args.template_file.relative_to(args.template_dir)
    template = env.get_template(str(template_file_name))
    data=yaml.safe_load(args.data_file.read_text())
    data["errors_sanitized"] = {}
    data['error_definitions'] = {}
    if args.errors_path and "errors" in data.keys():
        for err in data["errors"]:
            error_path = err['reference'].split('#')[0]

            filename = Path(args.errors_path, error_path.split('/')[-1])
            with open(filename.with_suffix(".yaml")) as f:
                text = f.read()
                yaml_content = yaml.safe_load(text)
            data['error_definitions'][error_path] = {}
            for err_def in yaml_content['errors']:
                data['error_definitions'][error_path][err_def['name']] = err_def['description']

            if not error_path in data["errors_sanitized"]:
                data["errors_sanitized"][error_path] = []
            if len(err['reference'].split('#')) > 1:
                data["errors_sanitized"][error_path].append(err['reference'].split('#')[1][1:])
            else:
                for error in data['error_definitions'][error_path].keys():
                    data["errors_sanitized"][error_path].append(error)
    output = template.render(
        name=args.name,
        handwritten_module_doc=args.module_handwritten_doc,
        data=data,
    )
    args.target_file.write_text(output)


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        exit(1)
