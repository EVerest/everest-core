from ..everest_ci import EverestCI
import dagger
from typing import Annotated

async def build_kit(
    docker_dir: dagger.Directory,
    base_image_tag: str,
) -> EverestCI.BuildKitResult:
        """
        Build the everest-core build kit container

        Parameters
        ----------
        docker_dir : dagger.Directory
            Directory containing the Dockerfile for the build kit.
        base_image_tag : str
            Base image tag to build the build-kit from.
        
        Returns
        -------
        EverestCI.BuildKitResult
            An object containing the container and exit code of the build operation.
        """

        result = await EverestCI().build_kit(
            docker_dir=docker_dir,
            base_image_tag=base_image_tag,
        )

        return result
