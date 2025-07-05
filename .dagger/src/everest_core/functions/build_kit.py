from ..everest_ci import EverestCI
import dagger
from typing import Annotated

async def build_kit(
    docker_dir: Annotated[dagger.Directory, dagger.Doc("Directory of the build kit dockerfile")],
    base_image_tag: Annotated[str, dagger.Doc("Base image tag to build the build-kit from")],
) -> EverestCI.BuildKitResult:
        """Build the everest-core build kit container"""

        result = await EverestCI().build_kit(
            docker_dir=docker_dir,
            base_image_tag=base_image_tag,
        )

        return result
