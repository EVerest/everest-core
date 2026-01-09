import argparse
import jinja2
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
    parser = argparse.ArgumentParser(description="Generate RST index from file list")
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
        '--apis',
        type=str,
        dest='apis',
        action='store',
        required=True,
        help='Comma separated list of api names'
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

    if not args.target_file.is_absolute():
        raise ValueError("Target file path must be absolute")
    if args.target_file.suffix != '.rst':
        raise ValueError("Target file must have a .rst extension")

    if not args.target_file.parent.exists():
        args.target_file.parent.mkdir(parents=True, exist_ok=True)

    # Split comma-separated string back into a list
    api_list = args.apis.split(',')

    # turn list into dict
    apis = []

    for api_name in api_list:
        if not api_name:
            continue  # Skip empty strings

        apis.append({"name": api_name, "path": api_name})

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
        apis=apis
    )
    args.target_file.write_text(output)


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        exit(1)
