import dagger
from dagger import object_type
from typing import Annotated
from ..everest_ci import BaseResultType

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
    source_dir: dagger.Directory,
    source_path: str,
    build_path: str,
    dist_path: str,
    workdir_path: str,
) -> InstallResult:
    """
    Installs the built artifacts from the build directory to the distribution directory.
    
    Parameters
    ----------
    container: dagger.Container
        The container in which the installation will be performed.
    source_dir: dagger.Directory
        The source directory containing the CMakeLists.txt and other source files.
    source_path: str
        The path inside the container of the source directory.
    build_path: str
        The path inside the container of the build directory.
    dist_path: str
        The path inside the container where the distribution directory will be created.
    workdir_path: str
        The working directory path inside the container.
        It is recommended to be the same as build_path or its parent directory.
    
    Returns
    -------
    InstallResult
        An object containing the container, exit code, and the distribution directory.
    """

    build_dir_exists = await (
        container
        .with_exec(["test", "-d", build_path], expect=dagger.ReturnType.ANY)
        .exit_code()
    ) == 0
    if not build_dir_exists:
        raise RuntimeError(f"Build directory {build_path} does not exist. Please build the project first.")

    container = await (
        container
        .with_mounted_directory(
            source_path,
            source_dir,
        )
        .with_workdir(workdir_path)
        .with_exec(
            [
                "cmake",
                "--install", build_path,
            ],
            expect=dagger.ReturnType.ANY,
        )
    )

    result = InstallResult(
        container=container,
        exit_code=await container.exit_code(),
        dist_dir=await container.directory(dist_path),
    )

    return result
