from ..everest_ci import BaseResultType
import dagger
from dagger import object_type
from typing import Annotated

from ..helpers.shared_container_actions import setup_cpm_cache

@object_type
class ConfigureResult(BaseResultType):
    """
    Result of the configure operation, based on EverestCI.BaseResultType.
    It contains the container, exit code and the cache directory after configuration.

    Attributes:
    -----------
    cache_cpm: dagger.Directory
        Directory containing the CPM cache for the CMake project.
    """

    cache_cpm: dagger.Directory = dagger.field()

async def configure_cmake_gcc(
    container: dagger.Container,
    source_dir: dagger.Directory,
    source_path: str,
    cache_dir: dagger.Directory,
    cache_path: str,
    build_path: str,
    workdir_path: str,
    dist_path: str,
    wheels_path: str,
) -> ConfigureResult:
    """
    Configure CMake with GCC in the given container
    Parameters
    ----------
    container : dagger.Container
        Container to run the operation.
    source_dir : dagger.Directory
        Source directory for the CMake project.
    source_path : str
        The path inside the container to the source directory.
    cache_dir : dagger.Directory
        The directory handle to use for caching
    cache_path : str
        The path inside the container to the cache directory.
    build_path : str
        The path inside the container to the build directory.
    workdir_path : str
        The path inside the container to the working directory.
    dist_path : str
        The path inside the container to the distribution directory.
    wheels_path : str
        The path inside the container to the wheels directory.

    Returns
    -------
    ConfigureResult
        An object containing the container, exit code, and build directory after configuration.
    """

    cpm_cache_dir = cache_dir.directory("./CPM")

    container = setup_cpm_cache(
        container=container,
        path=f"{cache_path}/CPM",
        volume="everest_cpm_cache",
        source=cpm_cache_dir,
    )

    container = await (
        container
        .with_mounted_directory(
            path=source_path,
            source=source_dir,
        )
        .with_workdir(
            path=workdir_path,
        )
        .with_exec(
            [
                    "cmake",
                    "-B", build_path,
                    "-S", source_path,
                    "-G", "Ninja",
                    "-D", "EVC_ENABLE_CCACHE=ON",
                    "-D", "ISO15118_2_GENERATE_AND_INSTALL_CERTIFICATES=OFF",
                    "-D", f"CMAKE_INSTALL_PREFIX={dist_path}",
                    "-D", f"WHEEL_INSTALL_PREFIX={wheels_path}",
                    "-D", "BUILD_TESTING=ON",
                    "-D", "EVEREST_ENABLE_COMPILE_WARNINGS=ON",
                    "-D", "FETCHCONTENT_QUIET=OFF"
            ],
            expect= dagger.ReturnType.ANY
        )
        .with_exec(
            [
                "cp", "-r",
                f"{cache_path}/CPM",
                "/cpm_cache"
            ]
        )
    )

    result = ConfigureResult(
        container=container,
        exit_code=await container.exit_code(),
        cache_cpm=container.directory(f"/cpm_cache"),
    )

    return result
