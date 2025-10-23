#!/usr/bin/env python3
import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description='Checks whether the 404.html file exists in the multiversion root directory')
    parser.add_argument(
        '--404-file',
        type=Path,
        dest='not_found_file',
        action='store',
        required=True,
        help='Path to the 404.html file'
    )
    args = parser.parse_args()

    if not args.not_found_file.is_absolute():
        raise ValueError("404.html file must be absolute")

    if args.not_found_file.is_file():
        print(f"✅ 404.html exists at {args.not_found_file}")
    else:
        print(f"❌ 404.html does not exist at {args.not_found_file}")
        print("   Please make sure that your sphinx stem includes the generation of some 404.html file.")
        exit(1)


if __name__ == "__main__":
    main()
