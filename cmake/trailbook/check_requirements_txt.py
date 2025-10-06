#!/usr/bin/env python3
import argparse
import sys
from importlib.metadata import version, PackageNotFoundError
import re
import subprocess

def parse_requirement(req_line: str):
    req_line = req_line.strip()
    if not req_line or req_line.startswith("#"):
        return None
    match = re.match(r"([a-zA-Z0-9_\-]+)==([0-9\.]+)", req_line)
    if match:
        return match.groups()
    return (req_line, None)

def check_requirements(file_path: str, fix_in_venv: bool = False):
    errors = []
    with open(file_path, "r") as f:
        for line in f:
            parsed = parse_requirement(line)
            if not parsed:
                continue
            pkg, req_version = parsed
            try:
                installed_version = version(pkg)
                if req_version and installed_version != req_version:
                    errors.append(f"{pkg}=={req_version} (installed: {installed_version})")
            except PackageNotFoundError:
                errors.append(f"{pkg}=={req_version or 'any version'} (not installed)")

    if fix_in_venv and errors:
        if sys.prefix != sys.base_prefix:            
            print(f"Attempting to fix requirements in the current venv: {sys.prefix}")
            subprocess.run([sys.executable, "-m", "pip", "install", "-r", file_path], check=True)
            return check_requirements(file_path, fix_in_venv=False)
        else:
            print("Not in a virtual environment. Cannot fix requirements automatically.")

    if not errors:
        print("✅ All requirements are met.")
    else:
        print("❌ There are unmet requirements:")
        for e in errors:
            print("   ", e)
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Checks if the packages in a requirements.txt are satisfied.")
    parser.add_argument("requirements_file", type=str, help="Path to the requirements.txt")
    parser.add_argument("--fix-in-venv", action="store_true", help="Run pip install -r in the current venv if there are unmet requirements")
    args = parser.parse_args()
    check_requirements(args.requirements_file, args.fix_in_venv)

if __name__ == "__main__":
    main()
