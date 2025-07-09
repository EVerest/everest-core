import dagger
from dagger import object_type
from ..everest_ci import BaseResultType
from typing import Annotated

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
    source_dir: dagger.Directory,
    source_path: str,
    build_path: str,
    workdir_path: str,
) -> UnitTestsResult:
    """
    Run unit tests in a Dagger container.
    This function using CMake to run the tests collected by CTest.
    
    Parameters
    ----------
    container : dagger.Container
        The Dagger container in which to run the tests.
    source_dir : dagger.Directory
        The source directory containing the CMake project.
    source_path : str
        The path inside the container to the source directory.
    build_path : str
        The path inside the container to the build directory.
        Should be the same as for building
    workdir_path : str
        The working directory path inside the container.
        Can be any, it is recommend to use the build directory or its parent.
    
    Returns
    -------
    UnitTestResult
        An object containing the container that ran the tests, the exit code,
        and the last test log file.
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
                "--build", build_path,
                "--target", "test",
            ],
            expect=dagger.ReturnType.ANY,
        )
    )
    
    result = UnitTestsResult(
        container=container,
        exit_code=await container.exit_code(),
        last_test_log=await container.file(
            f"{build_path}/Testing/Temporary/LastTest.log",
        )
    )

    return result
