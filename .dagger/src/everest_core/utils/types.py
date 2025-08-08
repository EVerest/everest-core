from dagger import (
    object_type,
    Doc,
    Directory,
)

from typing import (
    Annotated,
)


@object_type
class CIWorkspaceConfig:
    """
    Configuration for the CI workspace.
    This class defines the paths used in the CI workspace inside containers.

    Attributes:
    -----------
    workspace_path: str
        Path to the CI workspace directory. It should be an absolute path.
    source_path: str
        Path to the source code directory relative to the workspace or an
        absolute path.
    build_path: str
        Path to the build directory relative to the workspace or an absolute
        path.
    dist_path: str
        Path to the distribution directory relative to the workspace or an
        absolute path.
    wheels_path: str
        Path to the wheels directory relative to the workspace or an absolute
        path.
    cache_path: str
        Path to the cache directory relative to the workspace or an absolute
        path.
    artifacts_path: str
        Path to the artifacts directory relative to the workspace or an
        absolute path.
    """

    workspace_path: Annotated[
        str,
        Doc(
            "Path to the CI workspace directory. "
            "It should be an absolute path."
        )
    ] = "/workspace"
    source_path: Annotated[
        str,
        Doc(
            "Path to the source code directory relative to the workspace or "
            "an absolute path."
        )
    ] = "source"
    build_path: Annotated[
        str,
        Doc(
            "Path to the build directory relative to the workspace or an "
            "absolute path."
        )
    ] = "build"
    dist_path: Annotated[
        str,
        Doc(
            "Path to the distribution directory relative to the workspace or "
            "an absolute path."
        )
    ] = "dist"
    wheels_path: Annotated[
        str,
        Doc(
            "Path to the wheels directory relative to the workspace or an "
            "absolute path."
        )
    ] = "wheels"
    cache_path: Annotated[
        str,
        Doc(
            "Path to the cache directory relative to the workspace or an "
            "absolute path."
        )
    ] = "cache"
    artifacts_path: Annotated[
        str,
        Doc(
            "Path to the artifacts directory relative to the workspace or an "
            "absolute path."
        )
    ] = "artifacts"

    def __post_init__(self):
        if not self.workspace_path.startswith("/"):
            print(
                f"Warning: workspace_path should start with '/'. "
                f"Current value: {self.workspace_path}. "
                "Setting it to start with '/'"
            )
            self.workspace_path = f"/{self.workspace_path}"
        if not self.source_path.startswith("/"):
            self.source_path = f"{self.workspace_path}/{self.source_path}"
        if not self.build_path.startswith("/"):
            self.build_path = f"{self.workspace_path}/{self.build_path}"
        if not self.dist_path.startswith("/"):
            self.dist_path = f"{self.workspace_path}/{self.dist_path}"
        if not self.wheels_path.startswith("/"):
            self.wheels_path = f"{self.workspace_path}/{self.wheels_path}"
        if not self.cache_path.startswith("/"):
            self.cache_path = f"{self.workspace_path}/{self.cache_path}"
        if not self.artifacts_path.startswith("/"):
            self.artifacts_path = (
                f"{self.workspace_path}/{self.artifacts_path}"
            )


@object_type
class GithubConfig:
    """
    Configuration for GitHub Actions environment.
    This class defines the properties used in GitHub Actions.
    """
    is_github_run: Annotated[
        bool,
        Doc("Indicates if running in a GitHub Actions environment.")
    ] = False
    github_token: Annotated[
        str | None,
        Doc("GitHub token for authentication. None if not provided.")
    ] = None
    org_name: Annotated[
        str | None,
        Doc("GitHub organization name. None if not provided.")
    ] = None
    repo_name: Annotated[
        str | None,
        Doc("GitHub repository name. None if not provided.")
    ] = None
    sha: Annotated[
        str | None,
        Doc("GitHub commit SHA. None if not provided.")
    ] = None
    run_id: Annotated[
        str | None,
        Doc("GitHub Actions run ID. None if not provided.")
    ] = None
    attempt_number: Annotated[
        int | None,
        Doc("GitHub Actions attempt number. None if not provided.")
    ] = None
    event_name: Annotated[
        str | None,
        Doc("GitHub Actions event name. None if not provided.")
    ] = None


@object_type
class CIConfig:
    everest_ci_version: Annotated[
        str,
        Doc("Version of the Everest CI"),
    ]
    everest_dev_environment_docker_version: Annotated[
        str,
        Doc("Version of the Everest development environment Docker image"),
    ]
    source_dir: Annotated[
        Directory,
        Doc("Directory containing the source code"),
    ]
    cache_dir: Annotated[
        Directory,
        Doc("Directory containing the cache"),
    ]
    ci_workspace_config: Annotated[
        CIWorkspaceConfig,
        Doc("Configuration for the CI workspace"),
    ]
    github_config: Annotated[
        GithubConfig,
        Doc("Configuration for the GitHub Actions environment"),
    ]
