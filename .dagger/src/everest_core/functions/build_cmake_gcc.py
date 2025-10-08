import dagger
from dagger import object_type
from ..everest_ci import BaseResultType
from ..utils.common_container_actions import setup_ccache
from ..utils.types import CIConfig


@object_type
class BuildResult(BaseResultType):
    """
    Result of the build operation, based on EverestCI.BaseResultType.
    It contains the container, exit code, and the cache directory after
    the build operation.

    Attributes
    ----------
    cache_ccache : dagger.Directory
        Directory containing the ccache for the CMake project.
    """

    cache_ccache: dagger.Directory = dagger.field()


async def build_cmake_gcc(
    container: dagger.Container,
    ci_config: CIConfig,
) -> BuildResult:
    """
    Build a CMake project using GCC.

    Parameters
    ----------
    container : dagger.Container
        The container in which the build will be executed.
    ci_config : CIConfig
        The CI configuration object containing paths and directories.

    Returns
    -------
    BuildResult
        An object containing the container, exit code, and ccache directory
        after the build operation.
    """

    ccache_dir = ci_config.cache_dir.directory("./ccache")

    container = setup_ccache(
        container=container,
        path=f"{ci_config.ci_workspace_config.cache_path}/ccache",
        volume="everest_ccache",
        source=ccache_dir,
    )

    # Check if the build directory exists
    build_dir_exists = await (
        container
        .with_exec(
            ["test", "-d", ci_config.ci_workspace_config.build_path],
            expect=dagger.ReturnType.ANY,
        )
        .exit_code()
    ) == 0
    if not build_dir_exists:
        raise RuntimeError(
            (
                "Build directory "
                f"'{ci_config.ci_workspace_config.build_path}' "
                "does not exist. Please ensure that the build directory is "
                "created before running this function."
            )
        )

    container = await (
        container
        .with_mounted_directory(
            path=ci_config.ci_workspace_config.source_path,
            source=ci_config.source_dir,
        )
        .with_workdir(ci_config.ci_workspace_config.workspace_path)
        .with_exec(
            [
                "cmake",
                "--build", ci_config.ci_workspace_config.build_path,
            ],
            expect=dagger.ReturnType.ANY,
        )
        # .with_exec(
        #     [
        #         "cp", "-r",
        #         f"{ci_config.ci_workspace_config.cache_path}/ccache",
        #         "/ccache"
        #     ]
        # )
    )

    result = BuildResult(
        container=container,
        exit_code=await container.exit_code(),
        cache_ccache=container.directory("/ccache"),
    )

    return result
