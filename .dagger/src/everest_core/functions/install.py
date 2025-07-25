import dagger
from dagger import object_type
from typing import Annotated
from ..everest_ci import BaseResultType
from ..utils.types import CIConfig


@object_type
class InstallResult(BaseResultType):
    """
    Result type for the install operation.
    Inherits from `BaseResultType`. Which contains the container and exit code.
    Attributes:
        dist_dir: The directory where the installation is in.
    """
    dist_dir: Annotated[dagger.Directory, dagger.Doc("The directory where the installation is in")] = dagger.field()


async def install(
    container: dagger.Container,
    ci_config: CIConfig,
) -> InstallResult:
    """
    Installs the built artifacts from the build directory to the distribution directory.

    Parameters
    ----------
    container: dagger.Container
        The container in which the installation will be performed.
    ci_config: CIConfig
        The CI configuration object containing paths and directories.

    Returns
    -------
    InstallResult
        An object containing the container, exit code, and the distribution directory.
    """

    build_dir_exists = await (
        container
        .with_exec(["test", "-d", ci_config.ci_workspace_config.build_path], expect=dagger.ReturnType.ANY)
        .exit_code()
    ) == 0
    if not build_dir_exists:
        raise RuntimeError(f"Build directory {ci_config.ci_workspace_config.build_path} does not exist. Please build the project first.")

    container = await (
        container
        .with_mounted_directory(
            ci_config.ci_workspace_config.source_path,
            ci_config.source_dir,
        )
        .with_workdir(ci_config.ci_workspace_config.workspace_path)
        .with_exec(
            [
                "cmake",
                "--install", ci_config.ci_workspace_config.build_path,
            ],
            expect=dagger.ReturnType.ANY,
        )
    )

    result = InstallResult(
        container=container,
        exit_code=await container.exit_code(),
        dist_dir=await container.directory(ci_config.ci_workspace_config.dist_path),
    )

    return result
