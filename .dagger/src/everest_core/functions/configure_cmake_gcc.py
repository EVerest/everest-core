from ..everest_ci import BaseResultType
import dagger
from dagger import object_type
from typing import Annotated

from ..helpers.shared_container_actions import setup_cpm_cache

@object_type
class ConfigureResult(BaseResultType):
    """Result of the configure operation"""
    build_dir: Annotated[dagger.Directory, dagger.Doc("Directory where the build files are located")] = dagger.field()

async def configure_cmake_gcc(
    container: Annotated[dagger.Container, dagger.Doc("Container to run the operation")],
    source_dir: Annotated[dagger.Directory, dagger.Doc("Source directory for the CMake project")],
    cache_path: Annotated[str, dagger.Doc("Path to the cache directory")],
    source_path: Annotated[str, dagger.Doc("Source path for the CMake project")],
    build_path: Annotated[str, dagger.Doc("Build directory path for the CMake project")],
    workdir_path: Annotated[str, dagger.Doc("Working directory path for container")],
    dist_path: Annotated[str, dagger.Doc("Path to the distribution directory")],
    wheels_path: Annotated[str, dagger.Doc("Path to the wheels directory")],
) -> ConfigureResult:
    """Configure CMake with GCC in the given container"""

    container = setup_cpm_cache(
        container=container,
        path=cache_path,
        volume="everest_cpm_cache",
        source=source_dir,
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
                    "-B", f"{build_path}",
                    "-S", f"{source_path}",
                    "-G", "Ninja",
                    "-D", "EVC_ENABLE_CCACHE=ON",
                    "-D", "ISO15118_2_GENERATE_AND_INSTALL_CERTIFICATES=OFF",
                    "-D", f"CMAKE_INSTALL_PREFIX={dist_path}",
                    "-D", f"WHEEL_INSTALL_PREFIX={wheels_path}",
                    "-D", "BUILD_TESTING=ON",
                    "-D", "EVEREST_ENABLE_COMPILE_WARNINGS=ON",
            ],
            expect= dagger.ReturnType.ANY
        )
    )

    result = ConfigureResult(
        container=container,
        exit_code=await container.exit_code(),
        build_dir=await container.directory(build_path)
    )

    return result

