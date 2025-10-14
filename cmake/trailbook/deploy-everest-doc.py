import argparse
from typing import List
import jinja2
from pathlib import Path
import shutil

def main():
    parser = argparse.ArgumentParser(description='Process versions_index.html.jinja')
    parser.add_argument(
        '--template-file-path',
        type=Path,
        dest='template_file_path',
        action='store',
        required=True,
        help="Template file path"
    )
    parser.add_argument(
        '--multiversion-root-directory',
        type=Path,
        dest='multiversion_root_directory',
        action='store',
        required=True,
        help="Multiversion root directory"
    )
    args = parser.parse_args()

    if not args.template_file_path.is_absolute():
        raise ValueError("Template file path must be absolute")
    if not args.template_file_path.exists():
        raise FileNotFoundError(f"Template file {args.template_file_path} does not exist")
    if not args.template_file_path.is_file():
        raise FileNotFoundError(f"Template file {args.template_file_path} is not a file")
    template_dir = args.template_file_path.parent

    if not args.multiversion_root_directory.is_absolute():
        raise ValueError("Multiversion root directory must be absolute")
    if not args.multiversion_root_directory.exists():
        raise FileNotFoundError(f"Multiversion root directory {args.multiversion_root_directory} does not exist")
    if not args.multiversion_root_directory.is_dir():
        raise NotADirectoryError(f"Multiversion root directory {args.multiversion_root_directory} is not a directory")
    target_file_path = args.multiversion_root_directory / "versions_index.html"
    if target_file_path.exists():
        raise FileExistsError(f"Target file {target_file_path} already exists. Will not overwrite.")

    version_list: List[str] = []
    version_list.extend(
        item.name for item in args.multiversion_root_directory.iterdir() if item.is_dir()
    )

    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(template_dir),
        trim_blocks=True,
        lstrip_blocks=True
    )
    template = env.get_template(args.template_file_path.name)
    output = template.render(
        version_list=version_list
    )
    args.target_file_path.write_text(output)

    shutil.copy(
        args.html_root_dir / args.version_name / "versions_index.html",
        args.html_root_dir / "versions_index.html"
    )

if __name__ == "__main__":
    main()
