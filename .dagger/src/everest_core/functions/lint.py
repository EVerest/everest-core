from ..everest_ci import EverestCI
import dagger
from typing import Annotated

async def lint(
    container: Annotated[dagger.Container, dagger.Doc("Container to run the linting on")],
    source_dir: Annotated[dagger.Directory, dagger.Doc("Directory to run linting on")],
) -> EverestCI.ClangFormatResult:
    """Run linting on the given container"""

    result = await EverestCI().clang_format(
        container=container,
        source_dir=source_dir,
    )

    return result
