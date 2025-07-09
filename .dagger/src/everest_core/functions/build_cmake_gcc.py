import dagger
from dagger import object_type
from ..everest_ci import BaseResultType
from ..helpers.shared_container_actions import setup_ccache

@object_type
class BuildResult(BaseResultType):
    """
    Result of the build.
    
    Inherits from BaseResultType to include common attributes like container and exit_code.
    """
    pass

async def build_cmake_gcc(
    container: dagger.Container,
    source_dir: dagger.Directory,
    source_path: str,
    build_path: str,
    workdir_path: str,
    cache_dir: dagger.Directory | None,
    cache_path: str,
) -> BuildResult:
    """
    Build a CMake project using GCC.

    Parameters
    ----------
    container : dagger.Container
        The container in which the build will be executed.
    source_dir : dagger.Directory
        The directory containing the source code.
    source_path : str
        The path inside the container to the source code.
    build_path : str
        The path inside the container to the build directory. Should be the same as for configuration.
    workdir_path : str
        The working directory path where the build will be executed.
        Can be anyone, but it is recommended to use the build directory or its parent.

    Returns
    -------
    BuildResult
        An object containing the container, exit code, and build directory after the build operation.
    """

    # Retrieve the ccache directory from the cache directory if provided
    ccache_dir = cache_dir.directory("./ccache") if cache_dir else None

    container = setup_ccache(
        container=container,
        path=f"{ cache_path }/ccache",
        volume="everest_ccache",
        source=ccache_dir,
    )

    # Check if the build directory exists
    build_dir_exists = await (
        container
        .with_exec(["test", "-d", build_path], expect=dagger.ReturnType.ANY)
        .exit_code()
    ) == 0
    if not build_dir_exists:
        raise RuntimeError(
            f"Build directory '{build_path}' does not exist. "
            "Please ensure that the build directory is created before running this function."
        )

    container = await (
        container
        .with_mounted_directory(
            path=source_path,
            source=source_dir,
        )
        .with_workdir(workdir_path)
        .with_exec(
            [
                "cmake",
                "--build", build_path,
            ],
            expect=dagger.ReturnType.ANY,
        )
    )

    result = BuildResult(
        container=container,
        exit_code=await container.exit_code(),
    )

    return result
