import dagger
from dagger import object_type
from ..everest_ci import BaseResultType
from typing import Annotated
from ..utils.types import CIConfig


@object_type
class UnitTestsResult(BaseResultType):
    """
    Result type for unit tests.
    This class extends `BaseResultType` and includes a field for the last test log file.

    Attributes
    ----------
    last_test_log : dagger.File
        The file containing the last test log.
    """

    last_test_log: Annotated[dagger.File, dagger.Doc("Last test log file")] = dagger.field()


async def unit_tests(
    container: dagger.Container,
    ci_config: CIConfig,
) -> UnitTestsResult:
    """
    Run unit tests in a Dagger container.
    This function using CMake to run the tests collected by CTest.
    
    Parameters
    ----------
    container : dagger.Container
        The Dagger container in which to run the tests.
    ci_config : CIConfig
        The CI configuration object containing paths and directories.

    Returns
    -------
    UnitTestResult
        An object containing the container that ran the tests, the exit code,
        and the last test log file.
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
                "--build", ci_config.ci_workspace_config.build_path,
                "--target", "test",
            ],
            expect=dagger.ReturnType.ANY,
        )
    )
    
    result = UnitTestsResult(
        container=container,
        exit_code=await container.exit_code(),
        last_test_log=await container.file(
            f"{ci_config.ci_workspace_config.build_path}/Testing/Temporary/LastTest.log",
        )
    )

    return result
