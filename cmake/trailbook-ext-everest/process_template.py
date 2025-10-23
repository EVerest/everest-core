
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
        '--has-module-explanation',
        dest='has_module_explanation',
        action='store_true',
        help='Flag indicating if the module explanation should be referenced'
    )
    parser.add_argument(
        '--target-file',
        type=Path,
        dest='target_file',
        action='store',
        required=True,
        help='Output file for the processed template'
    )
    parser.set_defaults(
        has_module_explanation=False,
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
    if not args.target_file.parent.exists():
        raise ValueError("Target file directory does not exist")

    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(args.template_dir),
        trim_blocks=True,
        lstrip_blocks=True
    )
    env.filters['rst_indent'] = rst_indent
    env.filters['make_rst_ref'] = make_rst_ref

    template_file_name = args.template_file.relative_to(args.template_dir)
    template = env.get_template(str(template_file_name))
    output = template.render(
        name=args.name,
        data=yaml.safe_load(args.data_file.read_text()),
        has_module_explanation=args.has_module_explanation,
    )
    args.target_file.write_text(output)


if __name__ == "__main__":
    main()
