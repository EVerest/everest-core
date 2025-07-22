from ..everest_ci import EverestCI
import dagger
from ..utils.types import CIConfig


async def build_kit(
    docker_dir: dagger.Directory,
    ci_config: CIConfig,
) -> EverestCI.BuildKitResult:
    """
    Build the everest-core build kit container

    Parameters
    ----------
    docker_dir : dagger.Directory
        Directory containing the Dockerfile for the build kit.
    ci_config : CIConfig
        The CI configuration object containing paths and directories.

    Returns
    -------
    EverestCI.BuildKitResult
        An object containing the container and exit code of the build operation.
    """

    result = await EverestCI().build_kit(
        docker_dir=docker_dir,
        base_image_tag=ci_config.everest_ci_version,
    )

    return result
