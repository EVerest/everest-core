import argparse
import logging
import os

from . import git_handlers

log = logging.getLogger("EVerest's Development Tool")

def get_parser(version: str) -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter,
                                      description="EVerest's Development Tool",)
    parser.add_argument('--version', action='version', version=f'%(prog)s { version }')
    parser.add_argument('-v', '--verbose', action='store_true', help="Verbose output")
    parser.set_defaults(action_handler=lambda _: parser.print_help())

    subparsers = parser.add_subparsers(help="available commands")

    # Git related commands
    clone_parser = subparsers.add_parser("clone", help="Clone a repository", add_help=True)
    clone_parser.add_argument('-v', '--verbose', action='store_true', help="Verbose output")
    default_git_host = os.environ.get("EVEREST_DEV_TOOL_DEFAULT_GIT_HOST", "github.com")
    clone_parser.add_argument(
        '--host',
        default=default_git_host,
        help=(
            "Git host to use, default is 'github.com' "
            "(can be overridden by the environment variable "
            "EVEREST_DEV_TOOL_DEFAULT_GIT_HOST)"
        ),
    )
    default_git_method = os.environ.get("EVEREST_DEV_TOOL_DEFAULT_GIT_METHOD", "ssh")
    clone_parser.add_argument(
        '--method',
        default=default_git_method,
        choices=['https', 'ssh'],
        help=(
            "Git method to use, default is 'ssh' "
            "(can be overridden by the environment variable "
            "EVEREST_DEV_TOOL_DEFAULT_GIT_METHOD)"
        )
    )
    default_git_ssh_user = os.environ.get("EVEREST_DEV_TOOL_DEFAULT_GIT_SSH_USER", "git")
    clone_parser.add_argument(
        '--ssh-user',
        default=default_git_ssh_user,
        help=(
            "SSH user to use, default is 'git' "
            "(can be overridden by the environment variable "
            "EVEREST_DEV_TOOL_DEFAULT_GIT_SSH_USER)"
        )
    )
    default_git_organization = os.environ.get("EVEREST_DEV_TOOL_DEFAULT_GIT_ORGANIZATION", "EVerest")
    clone_parser.add_argument(
        '--organization', '--org',
        default=default_git_organization,
        help=(
            "Github Organization name, default is 'EVerest'"
            " (can be overridden by the environment variable "
            "EVEREST_DEV_TOOL_DEFAULT_GIT_ORGANIZATION)"
        )
    )
    clone_parser.add_argument('--branch', '-b', default="main", help="Branch to checkout, default is 'main'")
    clone_parser.add_argument('--dry', action='store_true', help="Dry run, do not execute the clone command")
    clone_parser.add_argument("repository_name", help="Name of the repository to clone")
    clone_parser.set_defaults(action_handler=git_handlers.clone_handler)

    return parser

def setup_logging(verbose: bool):
    if verbose:
        log.setLevel(logging.DEBUG)
    else:
        log.setLevel(logging.INFO)
    console_handler = logging.StreamHandler()
    log.addHandler(console_handler)

def main(parser: argparse.ArgumentParser):
    args = parser.parse_args()
    args.logger = log

    setup_logging(args.verbose)

    args.action_handler(args)
