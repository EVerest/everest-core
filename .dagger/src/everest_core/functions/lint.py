from ..everest_ci import EverestCI
import dagger
from typing import Annotated
from ..utils.types import CIConfig


async def lint(
    container: Annotated[
        dagger.Container,
        dagger.Doc(
            "Container to run the linting on"
        ),
    ],
    ci_config: Annotated[
        CIConfig,
        dagger.Doc(
            "CI configuration containing paths and directories"
        ),
    ]
) -> EverestCI.ClangFormatResult:
    """
    Run linting on the given container

    Parameters
    ----------
    container : dagger.Container
        The container in which to run the linting.
    ci_config : CIConfig
        The CI configuration object containing paths and directories.

    Returns
    -------
    EverestCI.ClangFormatResult
        An object containing the container and the exit code of the linting operation.
    """

    result = await EverestCI().clang_format(
        container=container,
        source_dir=ci_config.source_dir,
    )

    return result
