import dagger
from dagger import object_type
from ..everest_ci import BaseResultType
from ..helpers.shared_container_actions import setup_ccache

@object_type
class BuildResult(BaseResultType):
    """
    Result of the build.
    
    Inherits from BaseResultType to include common attributes like container and exit_code.

    Attributes
    ----------
    build_dir : dagger.Directory
        Directory containing the build artifacts.
    """
    build_dir: dagger.Directory = dagger.field()

async def build_cmake_gcc(
    container: dagger.Container,
    source_dir: dagger.Directory,
    source_path: str,
    build_dir: dagger.Directory,
    build_path: str,
    workdir_path: str,
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
    build_dir : dagger.Directory
        The build directory. The project should already be configured in this directory.
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
    container = setup_ccache(
        container=container,
        path=f"{ cache_path }/ccache",
        volume="everest_ccache",
    )

    container = await (
        container
        .with_mounted_directory(
            path=source_path,
            source=source_dir,
        )
        .with_mounted_directory(
            path=build_path,
            source=build_dir,
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
        build_dir=await container.directory(build_path),
    )

    return result
