"Bazel related functions for edm_tool."
import yaml
from typing import List, Optional, Dict


def _format_optional_string(value: Optional[str]):
    """Formats a string value as a string literal (with quotes) or `None` if the value is None."""
    if value is None:
        return "None"
    return f'"{value}"'


def _is_commit(revision: str):
    # Revision is a commit if it is a hexadecimal 40-character string
    return len(revision) == 40 and all(c in "0123456789abcdef" for c in revision.lower())

def _get_depname_for_label(label: str) -> str:
    build, depname, bazel = label.split(":")[1].split(".")
    if build != "BUILD" or bazel != "bazel":
        raise ValueError(f"Invalid build file name: {label}")
    return depname

def _parse_build_file_labels(labels: Optional[List[str]]) -> Dict[str, str]:
    # For easier matching of build files with dependencies
    # we convert the list of build files:
    # ```
    # [
    #      "@workspace//path/to/build:BUILD.<depname>.bazel",
    #      ...
    # ]
    # ```
    # into a dictionary:
    # ```
    # {
    #      "<depname>": "@workspace//path/to/build:BUILD.<depname>.bazel",
    #      ...
    # }
    # ```
    # and check that all build files have proper names.
    if labels is None:
        return {}

    return dict((_get_depname_for_label(label), label) for label in labels)


def generate_deps(args):
    "Parse the dependencies.yaml and print content of *.bzl file to stdout."
    with open(args.dependencies_yaml, 'r', encoding='utf-8') as f:
        deps = yaml.safe_load(f)

    build_files = _parse_build_file_labels(args.build_file)

    for name in build_files:
        if name not in deps:
            raise ValueError(f"Build file {name} does not have a corresponding dependency in {args.dependencies_yaml}")

    print("""
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def edm_deps():""")

    for name, desc in deps.items():
        repo = desc["git"]
        # The parameter is called `git_tag` but it can be a tag or a commit
        revision = desc["git_tag"]
        tag = None
        commit = None

        if _is_commit(revision):
            commit = revision
        else:
            tag = revision

        build_file = build_files.get(name)

        print(
            f"""
    maybe(
        git_repository,
        name = "{name}",
        remote = "{repo}",
        tag = {_format_optional_string(tag)},
        commit = {_format_optional_string(commit)},
        build_file = {_format_optional_string(build_file)},
    )
"""
        )
