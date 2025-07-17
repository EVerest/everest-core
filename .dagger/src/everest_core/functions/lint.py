from ..everest_ci import EverestCI
import dagger
from typing import Annotated

async def lint(
    container: Annotated[dagger.Container, dagger.Doc("Container to run the linting on")],
    source_dir: Annotated[dagger.Directory, dagger.Doc("Directory to run linting on")],
) -> EverestCI.ClangFormatResult:
    """
    Run linting on the given container
    
    Parameters
    ----------
    container : dagger.Container
        The container in which to run the linting.
    source_dir : dagger.Directory
        The directory containing the source code to be linted.

    Returns
    -------
    EverestCI.ClangFormatResult
        An object containing the container and the exit code of the linting operation.
    """

    result = await EverestCI().clang_format(
        container=container,
        source_dir=source_dir,
    )

    return result
