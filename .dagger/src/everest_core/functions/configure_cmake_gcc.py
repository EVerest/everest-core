from ..everest_ci import BaseResultType
import dagger
from dagger import object_type

from ..utils.common_container_actions import setup_cpm_cache
from ..utils.types import CIConfig


@object_type
class ConfigureResult(BaseResultType):
    """
    Result of the configure operation, based on EverestCI.BaseResultType.
    It contains the container, exit code and the cache directory after configuration.

    Attributes
    ----------
    cache_cpm: dagger.Directory
        Directory containing the CPM cache for the CMake project.
    """

    cache_cpm: dagger.Directory = dagger.field()


async def configure_cmake_gcc(
    container: dagger.Container,
    ci_config: CIConfig,
) -> ConfigureResult:
    """
    Configure CMake with GCC in the given container
    Parameters
    ----------
    container : dagger.Container
        Container to run the operation.
    ci_config : CIConfig
        The CI configuration object containing paths and directories.

    Returns
    -------
    ConfigureResult
        An object containing the container, exit code, and build directory after configuration.
    """

    cpm_cache_dir = ci_config.cache_dir.directory("./CPM")

    container = setup_cpm_cache(
        container=container,
        path=f"{ci_config.ci_workspace_config.cache_path}/CPM",
        volume="everest_cpm_cache",
        source=cpm_cache_dir,
    )

    container = await (
        container
        .with_mounted_directory(
            path=ci_config.ci_workspace_config.source_path,
            source=ci_config.source_dir,
        )
        .with_workdir(
            path=ci_config.ci_workspace_config.workspace_path,
        )
        .with_exec(
            [
                    "cmake",
                    "-B", ci_config.ci_workspace_config.build_path,
                    "-S", ci_config.ci_workspace_config.source_path,
                    "-G", "Ninja",
                    "-D", "EVC_ENABLE_CCACHE=ON",
                    "-D", "ISO15118_2_GENERATE_AND_INSTALL_CERTIFICATES=OFF",
                    "-D", f"CMAKE_INSTALL_PREFIX={ci_config.ci_workspace_config.dist_path}",
                    "-D", f"WHEEL_INSTALL_PREFIX={ci_config.ci_workspace_config.wheels_path}",
                    "-D", "BUILD_TESTING=ON",
                    "-D", "EVEREST_ENABLE_COMPILE_WARNINGS=ON",
                    "-D", "FETCHCONTENT_QUIET=OFF"
            ],
            expect=dagger.ReturnType.ANY
        )
        # .with_exec(
        #     [
        #         "cp", "-r",
        #         f"{ci_config.ci_workspace_config.cache_path}/CPM",
        #         "/cpm_cache"
        #     ]
        # )
    )

    result = ConfigureResult(
        container=container,
        exit_code=await container.exit_code(),
        cache_cpm=container.directory("/cpm_cache"),
    )

    return result
