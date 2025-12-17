#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""Everest Dependency Manager."""
import argparse
import logging
import json
from typing import Tuple
from jinja2 import Environment, FileSystemLoader
import yaml
import os
from pathlib import Path, PurePath
import subprocess
import sys
import shutil
import multiprocessing
import requests
import re
import datetime

from edm_tool import bazel


log = logging.getLogger("edm")
edm_config_dir_path = Path("~/.config/everest").expanduser().resolve()
edm_config_path = edm_config_dir_path / "edm.yaml"
metadata_timeout_s = 10


class LocalDependencyCheckoutError(Exception):
    """Exception thrown when a dependency could not be checked out."""


def install_bash_completion(path=Path("~/.local/share/bash-completion")):
    """Install bash completion to a user provided path."""
    source_bash_completion_file_path = Path(__file__).parent / "edm-completion.bash"
    target_bash_completion_dir = path.expanduser()
    target_bash_completion_dir_file_path = target_bash_completion_dir / "edm.sh"
    bash_completion_in_home = Path("~/.bash_completion").expanduser()
    if not target_bash_completion_dir.exists():
        target_bash_completion_dir.expanduser().mkdir(parents=True, exist_ok=True)
    shutil.copy(source_bash_completion_file_path, target_bash_completion_dir_file_path)
    log.debug("Updated edm bash completion file")

    if not bash_completion_in_home.exists():
        with open(bash_completion_in_home, 'w', encoding='utf-8') as bash_completion_dotfile:
            bash_completion_dotfile.write("for bash_completion_file in ~/.local/share/bash-completion/* ; do\n"
                                          "    [ -f \"$bash_completion_file\" ] && . $bash_completion_file\n"
                                          "done")
            log.info(f"Updated \"{bash_completion_in_home}\" to point to edm bash completion "
                     f"in \"{target_bash_completion_dir}\"")
    else:
        log.warning(f"\"{bash_completion_in_home}\" exists, could not automatically install bash-completion")
        log.info("Please add the following entry to your .bashrc:")
        log.info(f". {target_bash_completion_dir}/edm.sh")


class Color:
    """Represents a subset of terminal color codes for use in log messages."""

    DEFAULT = ""
    CLEAR = "\033[0m"
    BLACK = "\033[30m"
    GREY = "\033[90m"
    WHITE = "\033[37m"
    RED = "\033[31m"
    GREEN = "\033[32m"
    BLUE = "\033[34m"
    YELLOW = "\033[33m"
    MAGENTA = "\033[35m"
    CYAN = "\033[36m"

    @classmethod
    def set_none(cls):
        """Remove the color codes for no-color mode."""
        Color.DEFAULT = ""
        Color.CLEAR = ""
        Color.BLACK = ""
        Color.GREY = ""
        Color.WHITE = ""
        Color.RED = ""
        Color.GREEN = ""
        Color.BLUE = ""
        Color.YELLOW = ""
        Color.MAGENTA = ""
        Color.CYAN = ""


class ColorFormatter(logging.Formatter):
    """Logging formatter that uses pre-configured colors for different logging levels."""

    def __init__(self, color=True, formatting_str="[%(name)s]: %(message)s"):
        """Initialize the ColorFormatter."""
        super().__init__()
        self.color = color
        if not color:
            Color.set_none()
        self.formatting_str = formatting_str
        self.colored_formatting_strings = {
            logging.DEBUG: self.build_colored_formatting_string(Color.GREY),
            logging.INFO: self.build_colored_formatting_string(Color.CLEAR),
            logging.WARNING: self.build_colored_formatting_string(Color.YELLOW),
            logging.ERROR: self.build_colored_formatting_string(Color.RED),
            logging.CRITICAL: self.build_colored_formatting_string(Color.MAGENTA),
        }

    def build_colored_formatting_string(self, color: Color) -> str:
        """Build a formatting string with the provided color."""
        if self.color:
            return f"{color}{self.formatting_str}{Color.CLEAR}"
        return f"{self.formatting_str}"

    def format(self, record):
        """Format a record with the colored formatter."""
        return logging.Formatter(self.colored_formatting_strings[record.levelno]).format(record)


def quote(lst: list) -> list:
    """Put quotation marks around every list element, which is assumed to be a str."""
    return [f"\"{element}\"" for element in lst]


def prettify(lst: list, indent: int) -> str:
    """Construct string from list elements with the given indentation."""
    output = ""
    space = " " * indent
    for out in lst:
        if out and out != "\n":
            if len(output) > 0:
                output += f"\n{space}{out}"
            else:
                output += f"{space}{out}"
    return output


def pretty_print(lst: list, indent: int, log_level: int):
    """Debug log every list element with the given indentation."""
    space = " " * indent
    for out in lst:
        if out and out != "\n":
            if log_level == logging.DEBUG:
                log.debug(f"{space}{out}")
            elif log_level == logging.INFO:
                log.info(f"{space}{out}")
            elif log_level == logging.WARNING:
                log.warning(f"{space}{out}")
            elif log_level == logging.ERROR:
                log.error(f"{space}{out}")
            elif log_level == logging.CRITICAL:
                log.critical(f"{space}{out}")
            else:
                log.info(f"{space}{out}")


def pretty_print_process(c: subprocess.CompletedProcess, indent: int, log_level: int):
    """Pretty print stdout and stderr of a CompletedProcess object."""
    stdout = c.stdout.decode("utf-8").split("\n")
    pretty_print(stdout, indent, log_level)

    stderr = c.stderr.decode("utf-8").split("\n")
    pretty_print(stderr, indent, log_level)


def pattern_matches(string: str, patterns: list) -> bool:
    """Return true if one of the patterns match with the string, false otherwise."""
    matches = False
    for pattern in patterns:
        if PurePath(string).match(pattern):
            log.debug(f"Pattern \"{pattern}\" accepts string \"{string}\"")
            matches = True
            break
    return matches


class GitInfo:
    """Provide information about git repositories."""

    @classmethod
    def is_repo(cls, path: Path) -> bool:
        """Return true if path is a top-level git repo."""
        try:
            result = subprocess.run(["git", "-C", path, "rev-parse", "--git-dir"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            repo_dir = result.stdout.decode("utf-8").replace("\n", "")
            if repo_dir == ".git":
                return True
        except subprocess.CalledProcessError:
            return False
        return False

    @classmethod
    def is_dirty(cls, path: Path) -> bool:
        """Use git diff to check if the provided directory has uncommitted changes, ignoring untracked files."""
        try:
            subprocess.run(["git", "-C", path, "diff", "--quiet", "--exit-code"],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            subprocess.run(["git", "-C", path, "diff", "--cached", "--quiet", "--exit-code"],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            return False
        except subprocess.CalledProcessError:
            return True

    @classmethod
    def is_detached(cls, path: Path) -> bool:
        """Check if the git repo at path is in detached HEAD state."""
        try:
            subprocess.run(["git", "-C", path, "symbolic-ref", "-q", "HEAD"],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            return False
        except subprocess.CalledProcessError:
            return True

    @classmethod
    def fetch(cls, path: Path) -> bool:
        """
        Return true if git-fetch was successful, false if not.

        TODO: distinguish between error codes?
        """
        log.debug(f"\"{path.name}\": fetching information from remote. This might take a few seconds.")
        try:
            subprocess.run(["git", "-C", path, "fetch"],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            return True
        except subprocess.CalledProcessError as result:
            log.error(f"\"{path.name}\" Error during git-fetch: {result.returncode}")
            return False

    @classmethod
    def pull(cls, path: Path) -> bool:
        """
        Return true if git-pull was successful, false if not.

        TODO: distinguish between error codes?
        """
        log.info(f"\"{path.name}\": pulling from remote. This might take a few seconds.")
        try:
            subprocess.run(["git", "-C", path, "pull"],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            return True
        except subprocess.CalledProcessError as result:
            pretty_stderr = prettify(result.stderr.decode("utf-8").split("\n"), 4)
            log.error(f"\"{path.name}\" Error during git-pull: {result.returncode}\n{pretty_stderr}")
            return False

    @classmethod
    def get_behind(cls, path: Path) -> str:
        """Return how many commits behind the repo at path is relative to remote."""
        behind = ""
        try:
            result = subprocess.run(["git", "-C", path, "rev-list", "--count", "HEAD..@{u}"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            behind = result.stdout.decode("utf-8").replace("\n", "")
        except subprocess.CalledProcessError:
            return behind

        return behind

    @classmethod
    def get_ahead(cls, path: Path) -> str:
        """Return how many commits ahead the repo at path is relative to remote."""
        ahead = ""
        try:
            result = subprocess.run(["git", "-C", path, "rev-list", "--count", "@{u}..HEAD"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            ahead = result.stdout.decode("utf-8").replace("\n", "")
        except subprocess.CalledProcessError:
            return ahead

        return ahead

    @classmethod
    def get_tag(cls, path: Path) -> str:
        """Return the current tag of the repo at path, or an empty str."""
        tag = ""
        try:
            result = subprocess.run(["git", "-C", path, "describe", "--exact-match", "--tags"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            tag = result.stdout.decode("utf-8").replace("\n", "")
        except subprocess.CalledProcessError:
            return tag

        return tag

    @classmethod
    def get_branch(cls, path: Path) -> str:
        """Return the current branch of the repo at path, or an empty str."""
        branch = ""
        try:
            result = subprocess.run(["git", "-C", path, "symbolic-ref", "--short", "-q", "HEAD"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            branch = result.stdout.decode("utf-8").replace("\n", "")
        except subprocess.CalledProcessError:
            return branch

        return branch

    @classmethod
    def infer_branches(cls, path: Path) -> list:
        """If in detached HEAD mode, return the likely branches of the repo at path, or an empty list."""
        branches = []
        try:
            result = subprocess.run(["git", "-C", path, "branch", '--format="%(refname:lstrip=0)"', "--remotes", "--no-abbrev", "--contains"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            lines = result.stdout.decode("utf-8").splitlines()
            for line in lines:
                branch = line.strip("\"").replace("refs/remotes/origin/", "")
                if branch != "HEAD":
                    branches.append(branch)
        except subprocess.CalledProcessError:
            return branches

        return branches

    @classmethod
    def get_remote_branch(cls, path: Path) -> str:
        """Return the remote of the current branch of the repo at path, or an empty str."""
        remote_branch = ""
        try:
            result = subprocess.run(["git", "-C", path, "rev-parse", "--abbrev-ref", "--symbolic-full-name", "@{u}"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            remote_branch = result.stdout.decode("utf-8").replace("\n", "")
        except subprocess.CalledProcessError:
            return remote_branch

        return remote_branch

    @classmethod
    def get_remote_url(cls, path: Path) -> str:
        """Return the remote url of the repo at path, or an empty str."""
        remote_url = ""
        try:
            result = subprocess.run(["git", "-C", path, "config", "--get", "remote.origin.url"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            remote_url = result.stdout.decode("utf-8").replace("\n", "")
        except subprocess.CalledProcessError:
            return remote_url

        return remote_url

    @classmethod
    def get_remote_tags(cls, remote_url: str) -> list:
        """Return the remote tags of the repo at path, or an empty list."""
        remote_tags = []
        try:
            result = subprocess.run(["git", "-c", "versionsort.suffix=-", "ls-remote", "--tags", "--sort=-v:refname", "--refs", "--quiet", remote_url],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            result_list = result.stdout.decode("utf-8").split("\n")
            for entry in result_list:
                ref_and_tag = entry.split("\t")
                if len(ref_and_tag) > 1:
                    remote_tags.append(ref_and_tag[1].replace("refs/tags/", ""))
        except subprocess.CalledProcessError:
            return remote_tags

        return remote_tags

    @classmethod
    def get_remote_branches(cls, remote_url: str) -> list:
        """Return the remote branches of the repo at path, or an empty list."""
        remote_branches = []
        try:
            result = subprocess.run(["git", "ls-remote", "--heads", "--quiet", remote_url],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            result_list = result.stdout.decode("utf-8").split("\n")
            for entry in result_list:
                ref_and_tag = entry.split("\t")
                if len(ref_and_tag) > 1:
                    remote_branches.append(ref_and_tag[1].replace("refs/heads/", ""))
        except subprocess.CalledProcessError:
            return remote_branches

        return remote_branches

    @classmethod
    def get_current_rev(cls, path: Path) -> str:
        """Return the currently checked out ref of the repo at path, or an empty str."""
        rev = ""
        try:
            result = subprocess.run(["git", "-C", path, "rev-parse", "HEAD"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            rev = result.stdout.decode("utf-8").replace("\n", "")
        except subprocess.CalledProcessError:
            return rev

        return rev

    @classmethod
    def get_current_short_rev(cls, path: Path) -> str:
        """Return the currently checked out short ref of the repo at path, or an empty str."""
        rev = ""
        try:
            result = subprocess.run(["git", "-C", path, "rev-parse", "--short", "HEAD"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            rev = result.stdout.decode("utf-8").replace("\n", "")
        except subprocess.CalledProcessError:
            return rev

        return rev

    @classmethod
    def is_tag(cls, remote: str, tag: str) -> bool:
        """Return True if the given tag can be found on the given remote."""
        try:
            subprocess.run(["git", "ls-remote", "--exit-code", remote, f"refs/tags/{tag}"],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            return True
        except subprocess.CalledProcessError as called_process_error:
            if called_process_error.returncode == 2:
                return False
        return True

    @classmethod
    def get_rev(cls, remote: str, branch: str) -> str:
        """Return the rev of the given branch on the given remote or the branch name on error."""
        try:
            result = subprocess.run(["git", "ls-remote", "--exit-code", remote, branch],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            rev = result.stdout.decode("utf-8").replace("\n", "")
            return rev.split()[0]
        except subprocess.CalledProcessError:
            return branch

    @classmethod
    def get_git_repo_info(cls, repo_path: Path, fetch=False) -> dict:
        """
        Return useful information about a repository a the given path.

        Returns a default dictionary if the path is no git repo
        """
        repo_info = {
            'is_repo': False,
            'fetch_worked': None,
            'remote_branch': None,
            'behind': None,
            'ahead': None,
            'tag': None,
            'branch': None,
            'dirty': None,
            'detached': None,
            'rev': None,
            'short_rev': None,
            'url': None,
        }
        if GitInfo.is_repo(repo_path):
            repo_info["is_repo"] = True
            if fetch:
                repo_info["fetch_worked"] = GitInfo.fetch(repo_path)
            repo_info["remote_branch"] = GitInfo.get_remote_branch(repo_path)
            repo_info["behind"] = GitInfo.get_behind(repo_path)
            repo_info["ahead"] = GitInfo.get_ahead(repo_path)
            repo_info["tag"] = GitInfo.get_tag(repo_path)
            repo_info["branch"] = GitInfo.get_branch(repo_path)
            repo_info["dirty"] = GitInfo.is_dirty(repo_path)
            repo_info["detached"] = GitInfo.is_detached(repo_path)
            repo_info["rev"] = GitInfo.get_current_rev(repo_path)
            repo_info["short_rev"] = GitInfo.get_current_short_rev(repo_path)
            repo_info["url"] = GitInfo.get_remote_url(repo_path)
        return repo_info

    @classmethod
    def get_git_info(cls, path: Path, fetch=False) -> dict:
        """
        Return useful information about a repository a the given path.

        TODO: return type should be a well defined object
        Returns an empty dictionary if the path is no git repo
        """
        git_info = {}
        subdirs = list(path.glob("*/"))
        for subdir in subdirs:
            subdir_path = Path(subdir)
            repo_info = GitInfo.get_git_repo_info(subdir_path, fetch)

            git_info[subdir] = repo_info
        return git_info

    @classmethod
    def pull_all(cls, path: Path, repos=None) -> dict:
        """Pull all repositories in the given path, or a specific list of repos."""
        git_info = {}
        subdirs = list(path.glob("*/"))
        for subdir in subdirs:
            subdir_path = Path(subdir)
            if repos is not None and len(repos) > 0 and subdir_path.name not in repos:
                log.debug(f"Skipping {subdir_path.name} because it is not in the list of provided repos.")
                continue
            pull_info = {'is_repo': False}
            if GitInfo.is_repo(subdir_path):
                pull_info["is_repo"] = True
                pull_info["pull_worked"] = GitInfo.pull(subdir_path)

            git_info[subdir] = pull_info
        return git_info

    @classmethod
    def checkout_rev(cls, checkout_dir: Path, rev: str):
        """Check out the given rev in the given checkout_dir"""
        try:
            result = subprocess.run(["git", "-C", checkout_dir, "checkout", rev],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            pretty_print_process(result, 4, logging.DEBUG)
        except subprocess.CalledProcessError as result:
            pretty_print_process(result, 4, logging.DEBUG)


class EDM:
    """Provide dependecy management functionality."""

    @classmethod
    def print_git_info(cls, git_info):
        dirty_count = 0
        repo_count = 0
        for path, info in git_info.items():
            if not info["is_repo"]:
                log.debug(f"\"{path.name}\" is not a git repository.")
                continue
            repo_count += 1
            tag_or_branch = ""
            if info["tag"]:
                tag_or_branch += f" @ tag: {info['tag']}"
            if info["branch"]:
                tag_or_branch += f" @ branch: {info['branch']}"

            remote_info = ""
            if info["detached"]:
                remote_info = f" [{Color.YELLOW}detached HEAD @ {info['rev']}{Color.CLEAR}]"
            else:
                if "branch" in info and "remote_branch" in info:
                    remote_info = (f" [remote: {Color.RED}{info['remote_branch']}{Color.CLEAR}]")
                    behind_ahead = ""
                    if "behind" in info and info["behind"] and info["behind"] != "0":
                        behind_ahead += f"behind {Color.RED}{info['behind']}{Color.CLEAR}"
                    if "ahead" in info and info["ahead"] and info["ahead"] != "0":
                        if behind_ahead:
                            behind_ahead += " "
                        behind_ahead += f"ahead {Color.GREEN}{info['ahead']}{Color.CLEAR}"
                    if behind_ahead:
                        remote_info += f" [{behind_ahead}]"
            dirty = f"[{Color.GREEN}clean{Color.CLEAR}]"
            if info["dirty"]:
                dirty = f"[{Color.RED}dirty{Color.CLEAR}]"
                dirty_count += 1

            log.info(f"\"{Color.GREEN}{path.name}{Color.CLEAR}\"{tag_or_branch}{remote_info} {dirty}")

        if dirty_count > 0:
            log.info(f"{dirty_count}/{repo_count} repositories are dirty.")

    @classmethod
    def show_git_info(cls, working_dir: Path, workspace: str, git_fetch: bool):
        """Log information about git repositories."""
        git_info_working_dir = working_dir
        if workspace:
            git_info_working_dir = Path(workspace).expanduser().resolve()
            log.info("Workspace provided, executing git-info in workspace")
        log.info(f"Git info for \"{git_info_working_dir}\":")
        if git_fetch:
            log.info("Using git-fetch to update remote information. This might take a few seconds.")
        git_info = GitInfo.get_git_info(git_info_working_dir, git_fetch)
        EDM.print_git_info(git_info)

    @classmethod
    def setup_workspace_from_config(cls, workspace: str, config: str, update: bool, create_vscode_workspace: bool):
        """Setup a workspace from the provided config, update an existing workspace if specified."""
        workspace_dir = Path(workspace).expanduser().resolve()

        config_path = Path(config).expanduser().resolve()
        if config_path.exists():
            log.info(f"Using config \"{config_path}\"")
        else:
            log.error(f"Config file \"{config_path}\" does not exists, stopping.")
            sys.exit(1)
        config = parse_config(config_path)
        try:
            workspace_checkout = setup_workspace(workspace_dir, config, update)
        except LocalDependencyCheckoutError:
            log.error("Could not setup workspace. Stopping.")
            sys.exit(1)
        # copy config into workspace
        try:
            config_destination_path = workspace_dir / "workspace-config.yaml"
            shutil.copyfile(config_path, config_destination_path)
            log.info(f"Copied config into \"{config_destination_path}\"")
        except shutil.SameFileError:
            log.info(f"Did not copy workspace config because source and destination are the same \"{config_path}\"")

        if create_vscode_workspace:
            create_vscode_workspace_file(workspace_dir, workspace_checkout)

    @classmethod
    def config_from_dependencies(cls, dependencies: dict, external_in_config: bool, include_remotes: list) -> dict:
        """Assemble a config from the given dependencies."""
        new_config = {}
        if external_in_config:
            new_config = {**new_config, **dependencies}
            log.debug("Including external dependencies in generated config.")
        else:
            for name, entry in dependencies.items():
                if pattern_matches(entry["git"], include_remotes):
                    log.debug(f"Adding \"{name}\" to config. ")
                    new_config[name] = entry
                else:
                    log.debug(f"Did not add \"{name}\" to generated config because it is an external dependency.")

        return new_config

    @classmethod
    def create_config(cls, working_dir: Path, new_config: dict, external_in_config: bool, include_remotes: list) -> dict:
        """Scan all first-level subdirectories in working_dir for git repositories that might have been missed."""
        for subdir in list(working_dir.glob("*/")):
            subdir_path = Path(subdir)
            name = subdir_path.name
            if name in new_config:
                log.debug(f"Skipping {name} which already is in config.")
                continue
            # FIXME: change this when we support alias info for a repo.
            # then this name might not be not equal to the dep name anymore
            if not subdir_path.is_dir():
                log.debug(f"Skipping {name} because it is not a directory.")
                continue
            log.debug(f"Checking {subdir_path}: {subdir_path.name}")

            entry = {}

            remote = GitInfo.get_remote_url(subdir_path)
            if not remote:
                log.warning(f"Skipping {name} because remote could not be determined.")
            log.debug(f"  remote: {remote}")
            if not external_in_config and not pattern_matches(remote, include_remotes):
                log.debug(f"Skipping {name} because it is an external dependency.")
                continue
            entry["git"] = remote
            # TODO: check if there already is another config entry with this remote
            try:
                branch_result = subprocess.run(["git", "-C", subdir_path, "symbolic-ref", "--short", "-q", "HEAD"],
                                               stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
                branch = branch_result.stdout.decode("utf-8").replace("\n", "")
                log.debug(f"  branch: {branch}")
                entry["git_tag"] = branch
            except subprocess.CalledProcessError:
                try:
                    tag_result = subprocess.run(["git", "-C", subdir_path, "describe", "--exact-match", "--tags"],
                                                stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
                    tag = tag_result.stdout.decode("utf-8").replace("\n", "")
                    log.debug(f"  tag: {tag}")
                    entry["git_tag"] = tag
                except subprocess.CalledProcessError:
                    log.warning(f"Skipping {name} because no branch or tag could be determined.")
                    continue
            new_config[name] = entry

        return new_config

    @classmethod
    def create_snapshot(cls, working_dir: Path, config_path: Path) -> dict:
        git_info = GitInfo.get_git_info(working_dir, False)

        config = parse_config(config_path)
        for path, info in git_info.items():
            if not info["is_repo"]:
                log.debug(f"{path.name} is not a repo, path: {path}")
                continue
            if path.name not in config:
                config[path.name] = {}
                config[path.name]["git"] = info["url"]
            config[path.name]["git_rev"] = info["rev"]
            if info["branch"] != "":
                config[path.name]["branch"] = info["branch"]
            else:
                branches = GitInfo.infer_branches(path)
                branch = ""
                if "main" in branches:
                    branch = "main"
                elif "everest" in branches:
                    branch = "everest"
                # prefer release branches over main
                release_branches = [item for item in branches if item.startswith("release/")]
                if len(release_branches) > 0:
                    release_branches.sort(reverse=True)
                    branch = release_branches[0]
                config[path.name]["branch"] = branch
            if info["tag"]:
                config[path.name]["git_tag"] = info["tag"]
        return config

    @classmethod
    def write_config(cls, new_config: dict, out_path: str, silent=False):
        """Write the given config to the given path."""
        new_config_path = Path(out_path).expanduser().resolve()
        for config_entry_name, _ in new_config.items():
            if not silent:
                log.info(f"Adding \"{Color.GREEN}{config_entry_name}{Color.CLEAR}\" to config.")
        with open(new_config_path, 'w', encoding='utf-8') as new_config_file:
            yaml.dump(new_config, new_config_file)
            if not silent:
                log.info(f"Successfully saved config \"{new_config_path}\".")

    @classmethod
    def pull(cls, working_dir: Path, repos: list):
        """Pull all repos in working_dir or a restricted list of repos when provided."""
        pull_info = GitInfo.pull_all(working_dir, repos)
        pull_error_count = 0
        repo_count = 0
        for path, info in pull_info.items():
            if info["is_repo"]:
                repo_count += 1
                pulled = f"[{Color.GREEN}pulled{Color.CLEAR}]"
                if not info["pull_worked"]:
                    pulled = f"[{Color.RED}error during git-pull{Color.CLEAR}]"
                    pull_error_count += 1

                log.info(f"\"{Color.GREEN}{path.name}{Color.CLEAR}\"{pulled}")
            else:
                log.debug(f"\"{path.name}\" is not a git repository.")
        if pull_error_count > 0:
            log.info(f"{pull_error_count}/{repo_count} repositories could not be pulled.")

    @classmethod
    def scan_dependencies(cls, working_dir: Path, include_deps: list, files_to_ignore: set = None) -> Tuple[dict, set]:
        """Scan working_dir for dependencies."""
        log.info(f"Scanning \"{working_dir}\" for dependencies.")
        dependencies_files = set(list(working_dir.glob("**/dependencies.yaml")) +
                                 list(working_dir.glob("**/dependencies.yml")))

        if files_to_ignore:
            dependencies_files.difference_update(files_to_ignore)

        dependencies = {}
        for dependencies_file in dependencies_files:
            if dependencies_file.is_file():
                # filter _deps folders
                if not include_deps:
                    relative_path = dependencies_file.relative_to(working_dir).parent.as_posix()
                    if "_deps/" in relative_path:
                        log.info(
                            f"Ignoring dependencies in \"{dependencies_file}\" "
                            f"because this file is located in a \"_deps\" subdirectory.")
                        continue
                log.info(f"Parsing dependencies file: {dependencies_file}")
                with open(dependencies_file, encoding='utf-8') as dep:
                    try:
                        dependencies_yaml = yaml.safe_load(dep)
                        if dependencies_yaml is not None:
                            dependencies = {**dependencies, **dependencies_yaml}
                    except yaml.YAMLError as e:
                        log.error(f"Error parsing yaml of \"{dependencies_file}\": {e}")

        return (dependencies, dependencies_files)

    @classmethod
    def parse_workspace_files(cls, workspace_files: list) -> dict:
        """Parse the given list of workspace_files and return a workspace dict when exactly one workspace file is in the list"""
        workspace = {}
        if len(workspace_files) == 1:
            workspace_file = Path(workspace_files[0]).expanduser().resolve()
            if workspace_file.is_file():
                log.info(f"Using workspace file: {workspace_file}")
                with open(workspace_file, encoding='utf-8') as wsp:
                    try:
                        workspace_yaml = yaml.safe_load(wsp)
                        if workspace_yaml is not None:
                            workspace = {**workspace, **workspace_yaml}
                    except yaml.YAMLError as e:
                        log.error(f"Error parsing yaml of {workspace_file}: {e}")
        return workspace

    @classmethod
    def parse_workspace_directory(cls, workspace_dir: Path) -> dict:
        """Parse the given workspace_dir for possible local dependencies"""
        workspace = {}
        workspace["local_dependencies"] = {}
        workspace["workspace"] = workspace_dir.as_posix()
        for entry in workspace_dir.iterdir():
            if not entry.is_dir():
                pass
            workspace["local_dependencies"][entry.name] = {}
            workspace["local_dependencies"][entry.name] = {"git_tag": GitInfo.get_branch(entry)}

        return workspace

    @classmethod
    def checkout_local_dependencies(cls, workspace: dict, workspace_arg: str, dependencies: dict) -> list:
        """Checkout local dependencies in the workspace."""
        checkout = []
        if "local_dependencies" in workspace:
            workspace_dir = None
            # workspace given by command line always takes precedence
            if workspace_arg is not None:
                workspace_dir = Path(workspace_arg).expanduser().resolve()
                log.info(f"Using workspace directory \"{workspace_dir}\" from command line.")
            elif "workspace" in workspace:
                workspace_dir = Path(workspace["workspace"]).expanduser().resolve()
            else:
                print("Cannot checkout requested dependencies without a workspace directory, stopping.")
                sys.exit(1)
            for name, entry in workspace["local_dependencies"].items():
                if name not in dependencies:
                    log.debug(f"{name}: listed in workspace, but not in dependencies. Ignoring.")
                    continue
                checkout_dir = workspace_dir / name
                git_tag = None
                if "git_tag" in dependencies[name]:
                    git_tag = dependencies[name]["git_tag"]
                if entry is not None and "git_tag" in entry:
                    git_tag = entry["git_tag"]
                checkout.append(checkout_local_dependency(
                    name, dependencies[name]["git"], git_tag, None, checkout_dir, True))

        return checkout

    @classmethod
    def write_cmake(cls, workspace: dict, checkout: list, dependencies: dict, out_file: Path):
        """Generate a CMake file containing the dependencies in the given out_file."""
        templates_path = Path(__file__).parent / "templates"
        env = Environment(
            loader=FileSystemLoader(templates_path),
            trim_blocks=True,
        )
        env.filters['quote'] = quote

        cpm_template = env.get_template("cpm.jinja")
        render = cpm_template.render({
            "dependencies": dependencies,
            "checkout": checkout,
            "workspace": workspace})

        with open(out_file, 'w', encoding='utf-8') as out:
            log.info(f"Saving dependencies in: {out_file}")
            out.write(render)

    @classmethod
    def check_github_key(cls) -> bool:
        """Checks if a public key is stored at github."""
        valid = False
        try:
            subprocess.run(["ssh", "-T", "git@github.com"],
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
        except subprocess.CalledProcessError as process_error:
            if process_error.returncode == 1:
                valid = True

        return valid

    @classmethod
    def write_config_from_scanned_dependencies(cls, working_dir: Path, include_deps: list,
                                               external_in_config: bool, include_remotes: list, config_path: Path):
        """Writes a config file from the scanned dependencies in working_dir"""
        (dependencies, _) = EDM.scan_dependencies(working_dir, include_deps)
        new_config = EDM.config_from_dependencies(dependencies, external_in_config, include_remotes)
        new_config = EDM.create_config(working_dir, new_config, external_in_config, include_remotes)
        EDM.write_config(new_config, config_path)


def checkout_local_dependency(name: str, git: str, git_tag: str, git_rev: str, checkout_dir: Path, keep_branch=False) -> dict:
    """
    Clone local dependency into checkout_dir.

    If the directory already exists only switch branches if the git repo is not dirty or keep_branch is False
    """
    def clone_dependency_repo(git: str, git_tag: str, checkout_dir: Path) -> None:
        """Clone given git repository at the given git_tag into checkout_dir."""
        git_clone_args = [git, checkout_dir]
        if git_tag:
            git_clone_args = ["--branch", git_tag, git, checkout_dir]
        else:
            log.debug("  No git-tag specified, cloning default branch.")
        git_clone_cmd = ["git", "clone"] + git_clone_args

        try:
            result = subprocess.run(git_clone_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            pretty_print_process(result, 4, logging.DEBUG)
        except subprocess.CalledProcessError as e:
            error_message = "   Error while cloning git repository during local dependency checkout:"
            log.warning(error_message)
            pretty_print(e.stderr.decode().strip().split("\n"), 6, logging.WARNING)
            raise LocalDependencyCheckoutError(error_message) from e

    log.info(f"Setting up dependency \"{Color.GREEN}{name}{Color.CLEAR}\" in workspace")
    log.debug(f"  git-remote: \"{git}\"")
    log.debug(f"  git-tag: \"{git_tag}\"")
    log.debug(f"  git-rev: \"{git_rev}\"")
    log.debug(f"  local directory: \"{checkout_dir}\"")
    git_tag_is_git_rev = False
    if checkout_dir.exists():
        log.debug(f"    ... the directory for dependency \"{name}\" already exists at \"{checkout_dir}\".")
        # check if git is dirty
        if GitInfo.is_dirty(checkout_dir):
            log.debug("    Repo is dirty, nothing will be done to this repo.")
        elif keep_branch:
            log.debug("    Keeping currently checked out branch.")
        else:
            # if the repo is clean we can safely switch branches
            if git_tag is not None:
                log.debug(f"    Repo is not dirty, checking out requested git tag \"{git_tag}\"")
                GitInfo.checkout_rev(checkout_dir, git_tag)
    else:
        try:
            clone_dependency_repo(git, git_tag, checkout_dir)
        except LocalDependencyCheckoutError as e:
            # maybe the given tag was actually a rev?
            if not git_rev:
                # assume git_tag is git_rev
                log.info(f"    No git_rev given, but git_tag \"{git_tag}\" might be a git_rev, trying to checkout this rev.")
                git_rev = git_tag
                git_tag = None
                clone_dependency_repo(git, git_tag, checkout_dir)
                git_tag_is_git_rev = True
            elif git_rev and git_tag:
                log.info(f"    Both git_rev and git_tag given, but git_tag \"{git_tag}\" might be a git_rev,"
                         f" trying to checkout git_rev \"{git_rev}\" instead.")
                git_tag = None
                clone_dependency_repo(git, git_tag, checkout_dir)
                git_tag_is_git_rev = True
            else:
                raise e

    if git_rev is not None:
        log.debug(f"    Checking out requested git rev \"{git_rev}\"")
        GitInfo.checkout_rev(checkout_dir, git_rev)
        if git_tag_is_git_rev:
            log.info(f"    Successfully checked out git_rev \"{git_rev}\" of dependency \"{Color.GREEN}{name}{Color.CLEAR}\"")

    return {"name": name, "path": checkout_dir, "git_tag": git_tag}


def parse_config(path: Path) -> dict:
    """Parse a config file in yaml format at the given path."""
    if path.is_file():
        with open(path, encoding='utf-8') as config_file:
            try:
                config_yaml = yaml.safe_load(config_file)
                if config_yaml is not None:
                    return config_yaml
            except yaml.YAMLError as e:
                print(f"Error parsing yaml of {config_file}: {e}")
    return {}


def setup_workspace(workspace_path: Path, config: dict, update=False) -> dict:
    """Setup a workspace at the given workspace_path using the given config."""
    log.info(f"Setting up workspace \"{workspace_path}\"")
    workspace_checkout = []
    for name, entry in config.items():
        checkout_dir = workspace_path / name
        git_tag = None
        git_rev = None
        if entry is not None:
            if "git_tag" in entry:
                git_tag = entry["git_tag"]
            if "git_rev" in entry:
                git_rev = entry["git_rev"]
        workspace_checkout.append(checkout_local_dependency(name, entry["git"], git_tag, git_rev, checkout_dir))

    log.info("Done.")
    return workspace_checkout


def create_vscode_workspace_file(workspace_path: Path, workspace_checkout: dict):
    """Create a VS Code compatible workspace file at the given workspace_path."""
    vscode_workspace_file_path = workspace_path / f"{workspace_path.name}.code-workspace"

    content = {}
    if vscode_workspace_file_path.exists():
        log.warning(
            f"VS Code workspace file \"{vscode_workspace_file_path}\" exists.")
        log.info("Updating VS Code workspace file.")
        with open(vscode_workspace_file_path, 'r', encoding='utf-8') as ws_file:
            content = json.load(ws_file)
    else:
        log.info(f"Creating VS Code workspace file at: {vscode_workspace_file_path}")
    if "folders" not in content:
        content["folders"] = []
    for entry in workspace_checkout:
        folder = entry["path"].name
        if not any(f["path"] == folder for f in content["folders"]):
            log.debug(f"Dependency \"{Color.GREEN}{folder}{Color.GREY}\" added to VS Code workspace file")
            content["folders"].append({"path": folder})
    with open(vscode_workspace_file_path, 'w', encoding='utf-8') as ws_file:
        json.dump(content, ws_file, indent="\t")


def load_edm_config():
    config = None
    if edm_config_path.exists():
        # load config if exists
        log.debug(f"Loading edm config from {edm_config_path}")
        with open(edm_config_path, encoding='utf-8') as edm_config_file:
            try:
                config = yaml.safe_load(edm_config_file)
            except yaml.YAMLError as e:
                log.error(f"Error parsing yaml of \"{edm_config_path}\": {e}")
    return config


def init_handler(args):
    """Handler for the edm init subcommand"""
    working_dir = Path(args.working_dir).expanduser().resolve()

    if args.workspace:
        log.info(f"Using provided workspace path \"{args.workspace}\"")
        working_dir = Path(args.workspace)

    config_path = working_dir / "workspace-config.yaml"

    if args.list:
        tags = GitInfo.get_remote_tags("https://github.com/EVerest/everest-core.git")
        log.info(f"Available everest-core releases: {', '.join(tags)}")
        sys.exit(0)

    if args.release:
        log.info(f"Checking if requested EVerest release \"{args.release}\" is available...")
    elif args.config:
        log.info(f"Using supplied config \"{args.config}\"")
        main_handler(args)
        return
    else:
        log.info("No release specified, checking for most recent stable version...")

    github_key_available = EDM.check_github_key()

    github_https_pefix = "https://github.com/EVerest/"
    github_git_prefix = "git@github.com:EVerest/"

    github_prefix = github_https_pefix

    if github_key_available:
        github_prefix = github_git_prefix

    everest_core = {"name": "everest-core", "repo": github_prefix + "everest-core.git", "release": args.release}
    everest_cmake = {"name": "everest-cmake", "repo": github_prefix + "everest-cmake.git", "release": None}
    everest_dev_environment = {"name": "everest-dev-environment",
                               "repo": github_prefix + "everest-dev-environment.git", "release": None}
    everest_utils = {"name": "everest-utils", "repo": github_prefix + "everest-utils.git", "release": None}

    for repo in [everest_core, everest_cmake, everest_dev_environment, everest_utils]:
        tags = GitInfo.get_remote_tags(repo["repo"])
        latest_tag = tags[0] if len(tags) > 0 else "main"

        if repo["release"]:
            if repo["release"] in tags:
                latest_tag = repo["release"]
                log.info(f"Requested release is available: {repo['release']}")
            else:
                branches = GitInfo.get_remote_branches(repo["repo"])
                if repo["release"] in branches:
                    latest_tag = repo["release"]
                    log.info(f"Requested branch is available: {repo['release']}")
                else:
                    log.error(f"Requested release is NOT available: {repo['release']}")
                    sys.exit(1)

        log.info(f"Using \"{Color.GREEN}{repo['name']}{Color.CLEAR}\" @ {latest_tag}")
        checkout_local_dependency(repo["name"], repo["repo"], latest_tag, None, working_dir / repo["name"], False)

    # now we have the basics, get the rest recursively
    iterations = 10
    old_snapshot = {}
    config = {}
    scanned_dependencies_files = set()
    for i in range(iterations):
        if i > 0:
            # only do recursive parsing if explicitly requested
            (dependencies, dependencies_files) = EDM.scan_dependencies(
                working_dir, args.include_deps, scanned_dependencies_files)
            scanned_dependencies_files.update(dependencies_files)
            new_config = EDM.config_from_dependencies(dependencies, args.external_in_config, args.include_remotes)
            new_config = EDM.create_config(working_dir, new_config, args.external_in_config, args.include_remotes)
            # merge config with new_config, overwrite github https prefix with git prefix
            for name, entry in new_config.items():
                if name not in config:
                    if github_key_available and "git" in entry and entry["git"].startswith(github_https_pefix):
                        entry["git"] = entry["git"].replace(github_https_pefix, github_git_prefix, 1)
                    config[name] = entry
                    checkout_dir = working_dir / name
                    git_tag = None
                    git_rev = None
                    if entry is not None:
                        if "git_tag" in entry:
                            git_tag = entry["git_tag"]
                        if "git_rev" in entry:
                            git_rev = entry["git_rev"]
                    checkout_local_dependency(name, entry["git"], git_tag, git_rev, checkout_dir)
            EDM.write_config(config, config_path, True)
            # EDM.setup_workspace_from_config(working_dir, config_path, False, False)

        snapshot = EDM.create_snapshot(working_dir, config_path)
        if snapshot == old_snapshot:
            log.info(f'Stopping recursive workspace setup early after {i+1} loops.')
            break
        old_snapshot = snapshot
    EDM.show_git_info(working_dir, None, False)

    # write config file
    edm_config_dir_path.mkdir(parents=True, exist_ok=True)
    config = load_edm_config()

    if not config:
        log.info("Config is None, creating new config")
        config = {}
        config["edm"] = {}  # for general workspace independent config settings
        config["workspaces"] = {}

    with open(edm_config_path, 'w', encoding='utf-8') as edm_config_file:
        workspace_name = working_dir.name
        config["edm"]["active_workspace"] = workspace_name
        config["workspaces"][workspace_name] = {}
        config["workspaces"][workspace_name]["path"] = working_dir.as_posix()
        yaml.dump(config, edm_config_file)
        log.info(f"Successfully saved edm config \"{edm_config_path}\".")


def list_handler(_args):
    """Handler for the edm list subcommand"""
    log.info("Listing workspaces")
    config = load_edm_config()

    if not config:
        log.info("No edm config found")
        sys.exit(0)

    for workspace_name, workspace_config in config["workspaces"].items():
        log.info(f"  {workspace_name} ({workspace_config['path']})")


def rm_handler(args):
    """Handler for the edm rm subcommand"""
    config = load_edm_config()

    if not config:
        log.error("No edm config found")
        sys.exit(0)

    workspace_name = args.workspace_name[0]

    if workspace_name in config["workspaces"]:
        log.info(f"Removing workspace {workspace_name} from config.")
        del config["workspaces"][workspace_name]

        # write config
        with open(edm_config_path, 'w', encoding='utf-8') as edm_config_file:
            config["edm"]["active_workspace"] = None
            yaml.dump(config, edm_config_file)
            log.info(f"Successfully saved edm config \"{edm_config_path}\".")


def git_info_handler(args):
    """Handler for the edm git info subcommand"""
    working_dir = Path(args.working_dir).expanduser().resolve()

    if not args.repo_name:
        log.info("No repo name specified, listing git info for every repo in the current workspace")
        EDM.show_git_info(working_dir, None, True)
    else:
        log.info(f"Only listing git info for {', '.join(args.repo_name)}")
        git_info = {}
        for repo_name in args.repo_name:
            repo_path = working_dir / repo_name
            repo_info = GitInfo.get_git_repo_info(repo_path, True)
            git_info[repo_path] = repo_info
        EDM.print_git_info(git_info)
    sys.exit(0)


def git_pull_handler(args):
    """Handler for the edm git pull subcommand"""
    working_dir = Path(args.working_dir).expanduser().resolve()

    if not args.repo_name:
        log.info("No repo name specified, pulling all repos in the current workspace")
        EDM.pull(working_dir, repos=None)
    else:
        EDM.pull(working_dir, repos=args.repo_name)


def snapshot_handler(args):
    """Handler for the edm snapshot subcommand"""
    working_dir = Path(args.working_dir).expanduser().resolve()

    config_path = working_dir / "workspace-config.yaml"

    if not config_path.exists():
        log.info(f'Workspace config does not exist, creating a new one at: {config_path}')
        EDM.write_config_from_scanned_dependencies(
            working_dir, args.include_deps, args.external_in_config, args.include_remotes, config_path)

    log.info(f"Creating snapshot: {args.snapshot_name}")

    iterations = 1
    if args.recursive:
        iterations = args.recursive
    old_snapshot = {}
    for i in range(iterations):
        if i > 0:
            # only do recursive parsing if explicitly requested
            EDM.write_config_from_scanned_dependencies(
                working_dir, args.include_deps, args.external_in_config, args.include_remotes, config_path)
            EDM.setup_workspace_from_config(working_dir, config_path, False, False)
        snapshot = EDM.create_snapshot(working_dir, config_path)
        EDM.write_config(snapshot, args.snapshot_name)
        if snapshot == old_snapshot:
            log.info(f'Stopping recursive snapshot generation early after {i+1} loops.')
            break
        old_snapshot = snapshot
    sys.exit(0)


def check_non_local_dependecy(dependency_item):
    name, dependency = dependency_item

    if "git" not in dependency or dependency["git"] is None:
        log.warning(f'Dependency "{name}": git is not set')
        return dependency_item

    if "git_tag" not in dependency or dependency["git_tag"] is None:
        log.warning(f'Dependency "{name}": git_tag is not set')
        return dependency_item

    known_branches = ["main", "master"]

    log.debug(f'Dependency "{name}": determining if "{dependency["git_tag"]}" is a tag')
    if dependency["git_tag"] in known_branches or not GitInfo.is_tag(dependency["git"], dependency["git_tag"]):
        log.info(f'Dependency "{name}": "{dependency["git_tag"]}" is not a tag, requesting remote rev')
        dependency["git_tag"] = GitInfo.get_rev(dependency["git"], dependency["git_tag"])
    else:
        log.info(f'Dependency "{name}": "{dependency["git_tag"]}" is a tag')

    return dependency_item


def check_origin_of_dependencies(dependencies, checkout):
    non_local_dependencies = {}

    # handle locally available dependencies and filter out non-local ones
    for name, dependency in dependencies.items():
        if "git" not in dependency:
            log.info(f'Dependency "{name}": Using package instead of git url')
            continue
        shortcut = False
        for checkout_dep in checkout:
            if checkout_dep["name"] == name:
                shortcut = True
        if shortcut:
            log.info(f'Dependency "{name}": available locally')
            continue

        # fall-through
        non_local_dependencies[name] = dependency

    with multiprocessing.Pool() as pool:
        modified_dependencies = pool.map(check_non_local_dependecy, non_local_dependencies.items())
        for name, dependency in modified_dependencies:
            dependencies[name] = dependency


def modify_dependencies_yaml(dependencies, modified_dependencies_yaml):
    for name, entry in modified_dependencies_yaml.items():
        if name not in dependencies:
            if "add" in entry:
                dependencies[name] = {}
            else:
                continue
        dependency = dependencies[name]
        if not entry:
            continue

        if "rename" in entry:
            new_name = entry["rename"]
            log.info(f'Dependency "{name}": Renaming to "{new_name}"')
            dependencies[new_name] = dependencies.pop(name)
            name = new_name
            dependency = dependencies[name]

        for modification_name, modification_entry in entry.items():
            if modification_name in dependency:
                if modification_entry:
                    log.info(f'Dependency "{name}": Changing "{modification_name}" to "{modification_entry}"')
                    dependency[modification_name] = modification_entry
                else:
                    log.info(f'Dependency "{name}": Deleting "{modification_name}"')
                    del dependency[modification_name]
            else:
                if modification_entry:
                    log.info(f'Dependency "{name}": Adding "{modification_name}" containing "{modification_entry}"')
                    dependency[modification_name] = modification_entry


def modify_dependencies(dependencies, modify_dependencies_file):
    log.info(f'Modifying dependencies with file: {modify_dependencies_file}')
    with open(modify_dependencies_file, encoding='utf-8') as modified_dependencies_file:
        try:
            modified_dependencies_yaml = yaml.safe_load(modified_dependencies_file)
            if modified_dependencies_yaml:
                modify_dependencies_yaml(dependencies, modified_dependencies_yaml)
        except yaml.YAMLError as e:
            log.error(f"Error parsing yaml of {modify_dependencies_file}: {e}")


def modify_dependencies_urls(dependencies, modify_dependencies_input):
    log.info(f"Modifying dependencies with input: {modify_dependencies_input}")

    dep_match = re.findall(r"\s*prefix=(\S*)\s*replace=(\S*)", modify_dependencies_input, re.MULTILINE)

    if len(dep_match) == 0:
        log.warning(f"Dependencies modifications could not be parsed, ignoring them.")
        return

    for _, dependency in dependencies.items():
        if "git" in dependency:
            original_dependency_git = dependency["git"]
            for (source, target) in dep_match:
                if original_dependency_git.startswith(source):
                    dependency["git"] = original_dependency_git.replace(
                        source, target, 1)
                    log.info(f"Replaced dependency git URL '{original_dependency_git}' with '{dependency['git']}'")


def populate_component(metadata_yaml, key, version):
    meta = {"description": "", "license": "unknown", "name": key}
    if key in metadata_yaml:
        meta_entry = metadata_yaml[key]
        meta['description'] = meta_entry.get('description', '')
        meta['license'] = meta_entry.get('license', 'unknown')
        meta["name"] = meta_entry.get("name", key)
    component = {'name': meta["name"], 'version': version,
                    'description': meta['description'], 'license': meta['license']}
    return component


def release_handler(args):
    """Handler for the edm release subcommand"""
    everest_core_path = Path(args.everest_core_dir)
    build_path = Path(args.build_dir)
    release_path = Path(args.out)

    metadata_yaml = {}
    metadata_file = os.environ.get('EVEREST_METADATA_FILE', None)
    metadata_url = "https://raw.githubusercontent.com/EVerest/everest-dev-environment/main/everest-metadata.yaml"

    if not metadata_file:
        metadata_path = build_path / "everest-metadata.yaml"
        if not metadata_path.exists():
            log.info("No metadata.yaml provided, downloading...")
            try:
                request = requests.get(metadata_url, allow_redirects=True, timeout=metadata_timeout_s)

                with open(metadata_path, 'wb') as metadata:
                    metadata.write(request.content)
            except requests.exceptions.RequestException as e:
                log.info(f"Could not download metadata file, creating release.json without metadata: {e}")
    else:
        metadata_path = Path(metadata_file)
    if metadata_path.exists():
        log.info(f"Using metadata file: {metadata_path}")
        with open(metadata_path, encoding='utf-8') as metadata_file:
            metadata_yaml_data = yaml.safe_load(metadata_file)
            if metadata_yaml_data:
                metadata_yaml = metadata_yaml_data

    cpm_modules_path = build_path / "CPM_modules"
    everest_core_repo_info = GitInfo.get_git_repo_info(everest_core_path)
    everest_core_repo_info_git_tag = "unknown"
    if everest_core_repo_info["rev"]:
        everest_core_repo_info_git_tag = everest_core_repo_info["rev"]
    if everest_core_repo_info["branch"]:
        everest_core_repo_info_git_tag = everest_core_repo_info["branch"] + "@" + everest_core_repo_info["short_rev"]
    if everest_core_repo_info["tag"]:
        everest_core_repo_info_git_tag = everest_core_repo_info["tag"]
    snapshot_yaml = {"everest-core": {"git_tag": everest_core_repo_info_git_tag}}
    for cpm_module_file_name in sorted(os.listdir(cpm_modules_path)):
        cpm_module_file = cpm_modules_path / cpm_module_file_name
        if not cpm_module_file.is_file():
            continue
        with open(cpm_module_file, encoding='utf-8', mode='r') as cpm_module:
            cpm_add_package_line = None
            for line in cpm_module:
                if line.startswith("CPMAddPackage("):
                    cpm_add_package_line = line.strip().replace("CPMAddPackage(\"", "").replace("\")", "")+";"
                    break
            name = None
            git_repo = None
            git_tag = None
            source_dir = None
            name_match = re.search("NAME;(.*?);", cpm_add_package_line)
            if name_match:
                name = name_match.group(1)
            git_repo_match = re.search("GIT_REPOSITORY;(.*?);", cpm_add_package_line)
            if git_repo_match:
                git_repo = git_repo_match.group(1)
            git_tag_match = re.search("GIT_TAG;(.*?);", cpm_add_package_line)
            if git_tag_match:
                git_tag = git_tag_match.group(1)
            source_dir_match = re.search("SOURCE_DIR;(.*?);", cpm_add_package_line)
            if source_dir_match:
                source_dir = source_dir_match.group(1)

            if not name:
                print("  no NAME found?")
                sys.exit(1)
            if not source_dir and not git_tag:
                print("  no source dir found, cannot determine git tag")
                sys.exit(1)

            if not git_repo and source_dir:
                repo_info = GitInfo.get_git_repo_info(source_dir)
                if repo_info["branch"]:
                    git_tag = repo_info["branch"] + "@" + repo_info["short_rev"]
                if repo_info["tag"]:
                    git_tag = repo_info["tag"]
                if repo_info["url"]:
                    git_repo = repo_info["url"]

            snapshot_yaml[name] = {"git_tag": git_tag}

    d = datetime.datetime.utcnow()
    now = d.isoformat("T") + "Z"
    channel = os.environ.get('EVEREST_UPDATE_CHANNEL', "unknown")
    include_all = os.environ.get('EVEREST_METADATA_INCLUDE_ALL', "no")

    release_json = {"channel": channel, "datetime": now,
                    "version": snapshot_yaml["everest-core"]["git_tag"], "components": []}

    snapshot_yaml = dict(sorted(snapshot_yaml.items(), key=lambda entry: (entry[0].swapcase())))

    for key in snapshot_yaml:
        entry = snapshot_yaml[key]
        component = populate_component(metadata_yaml, key, entry['git_tag'])
        release_json['components'].append(component)

    if include_all == "yes":
        for key in metadata_yaml:
            component = populate_component(metadata_yaml, key, '')
            exists = False
            for existing_component in release_json['components']:
                if existing_component["name"] == component["name"]:
                    exists = True
            if exists:
                continue
            release_json['components'].append(component)

    with open(release_path, 'w', encoding='utf-8') as release_file:
        release_file.write(json.dumps(release_json))

    sys.exit(0)


def main_handler(args):
    working_dir = Path(args.working_dir).expanduser().resolve()

    if args.install_bash_completion:
        install_bash_completion()
        sys.exit(0)

    if args.git_pull is not None:
        EDM.pull(working_dir, repos=args.git_pull)
        sys.exit(0)

    if args.git_info:
        EDM.show_git_info(working_dir, args.workspace, args.git_fetch)
        sys.exit(0)

    if not args.config and not args.cmake and not args.create_config and not args.create_snapshot:
        log.info("No --config, --cmake or --create-config parameter given, exiting.")
        sys.exit(0)

    if args.config:
        if not args.workspace:
            log.error("A workspace path must be provided if supplying a config. Stopping.")
            sys.exit(1)

        EDM.setup_workspace_from_config(args.workspace, args.config, False, args.create_vscode_workspace)
        sys.exit(0)

    if args.create_snapshot:
        log.info(f"Creating snapshot: {args.create_snapshot}")
        snapshot = EDM.create_snapshot(working_dir)
        EDM.write_config(snapshot, args.create_snapshot)
        sys.exit(0)

    if not args.cmake and not args.create_config:
        log.error("FIXME")
        sys.exit(1)

    out_file = Path(args.out).expanduser().resolve()

    (dependencies, _) = EDM.scan_dependencies(working_dir, args.include_deps)

    if args.create_config:
        log.info("Creating config")
        new_config = EDM.config_from_dependencies(dependencies, args.external_in_config, args.include_remotes)
        new_config = EDM.create_config(working_dir, new_config, args.external_in_config, args.include_remotes)
        EDM.write_config(new_config, args.create_config)
        sys.exit(0)

    if not args.cmake:
        log.error("Calling the dependency manager without the --config parameter indicates usage from a CMake script. "
                  "If this is intendend , please use the --cmake flag to explicitly request this functionality.")
        sys.exit(1)

    env_workspace = os.environ.get('EVEREST_EDM_WORKSPACE')
    workspace_dir = working_dir.parent
    if env_workspace:
        workspace_dir = Path(env_workspace).expanduser().resolve()
        log.info(f'Using workspace path set in EVEREST_EDM_WORKSPACE environment variable: {workspace_dir}')
    else:
        log.info(f'Using parent directory as workspace path: {workspace_dir}')

    workspace = EDM.parse_workspace_directory(workspace_dir)
    checkout = EDM.checkout_local_dependencies(workspace, args.workspace, dependencies)

    # Apply modifications from environment variables to the dependencies

    # Apply URL modifications to the dependencies
    env_modify_dependencies_urls = os.environ.get('EVEREST_MODIFY_DEPENDENCIES_URLS')
    if env_modify_dependencies_urls:
        modify_dependencies_urls(dependencies, env_modify_dependencies_urls)

    # Apply modifications of whole dependency entries, comming from an additional file
    env_modify_dependencies = os.environ.get('EVEREST_MODIFY_DEPENDENCIES')
    if env_modify_dependencies:
        modify_dependencies_file = Path(env_modify_dependencies).expanduser().resolve()
        if modify_dependencies_file.is_file():
            modify_dependencies(dependencies, modify_dependencies_file)

    check_origin_of_dependencies(dependencies, checkout)

    EDM.write_cmake(workspace, checkout, dependencies, out_file)


def get_parser(version) -> argparse.ArgumentParser:
    """Return the argument parser containing all command line options."""
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter,
                                     description="Everest Dependency Manager")
    parser.add_argument('--version', action='version', version=f'%(prog)s {version}')
    parser.add_argument(
        "--workspace", metavar='WORKSPACE',
        help="Directory in which source code repositories that are explicity requested are checked out.",
        required=False)
    parser.add_argument("--working_dir", metavar='WORKINGDIR', default=".",
                        help="Working directory, default is the current one.", required=False)
    parser.add_argument("--out", metavar='OUTFILENAME', default="dependencies.cmake",
                        help="Path of the file that will contain the generated CPM cmake information")
    parser.add_argument(
        "--include_deps", action='store_true',
        help="Include dependency files that are stored in \"_deps\" directories. "
             "Given that files in these directories are part of the in-tree source cache of CPM "
             "you probably almost never want to do this.")
    parser.add_argument(
        "--config", metavar='CONFIG',
        help="Path to a config file that contains the repositories that should be checked out into the workspace.",
        required=False)
    parser.add_argument(
        "--create-vscode-workspace", action="store_true",
        help="Create a VS Code workspace by saving a <workspace>.code-workspace file in the workspace folder.")
    parser.add_argument(
        "--cmake", action="store_true",
        help="Use this flag to indicate that the dependency manager was called from a CMake script.")
    parser.add_argument(
        "--verbose", action="store_true",
        help="Verbose output.")
    parser.add_argument(
        "--nocolor", action="store_true",
        help="No color output.")
    parser.add_argument(
        "--install-bash-completion", action="store_true",
        help="Install bash completion if possible.")
    parser.add_argument(
        "--create-config", metavar='CREATECONFIG',
        help="Creates a config file at the given path containing all dependencies from the working directory.",
        required=False)
    parser.add_argument(
        "--external-in-config", action="store_true",
        help="Include external dependencies in created config file.")
    parser.add_argument(
        "--include-remotes", metavar='INTERNAL',
        help="List of git remotes that are included in a created config file",
        nargs="*",
        default=["git@github.com:EVerest/*", "https://github.com/EVerest/*"],
        required=False)
    parser.add_argument(
        "--create-snapshot",
        help="Creates a config file at the given path containing all repositories from the working directory.",
        nargs="?",
        const="snapshot.yaml",
        required=False)
    parser.add_argument(
        "--git-info", action="store_true",
        help="Show information of git repositories in working_dir")
    parser.add_argument(
        "--git-fetch", action="store_true",
        help="Use git-fetch to get updated info from remote")
    parser.add_argument(
        "--git-pull",
        help="Use git-pull to pull all git repositories in working_dir",
        nargs="*",
        required=False)
    # TODO(kai): consider implementing interactive mode
    # parser.add_argument("--interactive", action='store_true',
    #                     help="Interactively ask which repositories should be checked out.")

    subparsers = parser.add_subparsers(help='available commands')

    init_parser = subparsers.add_parser('init', add_help=True)
    init_parser.add_argument(
        "--config", metavar='CONFIG',
        help="Path to a config file that contains the repositories that should be checked out into the workspace.",
        required=False)
    init_parser.add_argument(
        "--workspace", metavar='WORKSPACE',
        help="Directory in which source code repositories that are explicity requested are checked out.",
        required=False)
    init_parser.add_argument(
        "release",
        help="Release version requested, if empty the most recent stable release is assumed.",
        nargs="?")
    init_parser.set_defaults(action_handler=init_handler)
    init_parser.add_argument(
        "--list",
        action="store_true",
        help="List available everest-core versions.")

    list_parser = subparsers.add_parser('list', add_help=True)
    list_parser.set_defaults(action_handler=list_handler)

    rm_parser = subparsers.add_parser('rm', add_help=True)
    rm_parser.set_defaults(action_handler=rm_handler)
    rm_parser.add_argument(
        "workspace_name",
        help="Name of the workspace to remove",
        nargs=1)

    git_parser = subparsers.add_parser('git', add_help=True)
    git_subparsers = git_parser.add_subparsers(help='available git commands')

    git_info_parser = git_subparsers.add_parser('info', add_help=True)
    git_info_parser.add_argument(
        "repo_name",
        help="Name of the repo(s) to get info from",
        nargs="*")
    git_info_parser.set_defaults(action_handler=git_info_handler)

    git_pull_parser = git_subparsers.add_parser('pull', add_help=True)
    git_pull_parser.add_argument(
        "repo_name",
        help="Name of the repo(s) to pull",
        nargs="*")
    git_pull_parser.set_defaults(action_handler=git_pull_handler)

    snapshot_parser = subparsers.add_parser('snapshot', add_help=True)
    snapshot_parser.set_defaults(action_handler=snapshot_handler)
    snapshot_parser.add_argument(
        "snapshot_name",
        help="Name of the snapshot file",
        nargs="?",
        default="snapshot.yaml")
    snapshot_parser.add_argument(
        "--recursive",
        help="Recursively check out the snapshot",
        nargs="?",
        const=10,
        required=False)
    snapshot_parser.add_argument(
        "--config",
        help="Path to a snapshot config.",
        nargs="?",
        const="snapshot-config.yaml",
        required=False)

    release_parser = subparsers.add_parser('release', add_help=True)
    release_parser.set_defaults(action_handler=release_handler)
    release_parser.add_argument(
        "--everest-core-dir",
        help="Path to everest-core",
        nargs="?",
        default="everest-core")
    release_parser.add_argument(
        "--build-dir",
        help="Path to everest-core build dir",
        nargs="?",
        default="build")
    release_parser.add_argument(
        "--out",
        help="Path to release.json file",
        nargs="?",
        default="release.json")
    bazel_parser = subparsers.add_parser(
        "bazel",
        description="Convert dependencies.yaml file into a file that can be used in Bazel workspace.",
        add_help=True)
    bazel_parser.set_defaults(action_handler=bazel.generate_deps)
    bazel_parser.add_argument(
        "dependencies_yaml",
        type=Path,
        help="Path to dependencies.yaml")
    bazel_parser.add_argument(
        "-b", "--build-file",
        type=str,
        action="append",
        help="Bazel-style label for the build files into the deppendencies. " +
             "The format should be `@<workspace>//<path>:BUILD.<dependency-name>.bazel`." +
             "<dependency-name> should correspond to the name of the dependency in " +
             "the dependencies.yaml file. This option can be used multiple times." +
             "If not provided, Bazel will search for BUILD file in the repo itself.",
        required=False)

    parser.set_defaults(action_handler=main_handler)

    return parser


def setup_logging(verbose: bool, nocolor: bool):
    """Setup logging, choosing logger level and if colorful log output is requested."""
    if verbose:
        log.setLevel(level=logging.DEBUG)
    else:
        log.setLevel(level=logging.INFO)
    console_handler = logging.StreamHandler()
    console_handler.setFormatter(ColorFormatter(color=not nocolor))
    log.addHandler(console_handler)

    if not nocolor:
        log.debug(
            "Using \033[1;31mc\033[1;33mo\033[93ml\033[92mo\033[94mr\033[34mf\033[95mu\033[35ml\033[0m \033[1m"
            "output\033[0m")


def main(parser: argparse.ArgumentParser):
    """The main entrypoint of edm. Provides different functionality based on the given command line arguments."""
    args = parser.parse_args()

    setup_logging(args.verbose, args.nocolor)

    if not os.environ.get("CPM_SOURCE_CACHE"):
        log.warning("CPM_SOURCE_CACHE environment variable is not set, this might lead to unintended behavior.")

    args.action_handler(args)
