import argparse
import jinja2
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description='Process versions_index.html.jinja and place redirect.html in the output directory')
    parser.add_argument(
        '--redirect-template',
        type=Path,
        dest='redirect_template',
        action='store',
        required=True,
        help="Redirect jinja template file"
    )
    parser.add_argument(
        '--target-path',
        type=Path,
        dest='target_path',
        action='store',
        required=True,
        help="Target path for the output"
    )
    parser.add_argument(
        '--latest-release-name',
        type=str,
        dest='latest_release_name',
        action='store',
        default="latest",
        help="Name of the latest release"
    )
    args = parser.parse_args()

    if not args.redirect_template.is_absolute():
        raise ValueError("Redirect template path must be absolute")
    if not args.redirect_template.exists():
        raise FileNotFoundError(
            "Redirect template path: '"
            + str(args.redirect_template)
            + "' doesn't exist"
        )
    if not args.redirect_template.is_file():
        raise FileNotFoundError(
            f"Redirect template path: '{args.redirect_template}' is not a file"
        )

    template_dir = args.redirect_template.parent
    template_name = args.redirect_template.name

    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(template_dir),
        trim_blocks=True,
        lstrip_blocks=True
    )

    template = env.get_template(template_name)
    output = template.render(
        latest_release=args.latest_release_name
    )
    args.target_path.write_text(output)


if __name__ == "__main__":
    main()
