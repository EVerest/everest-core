import dagger
from dagger import object_type
from typing import Annotated

@object_type
class Config:
    """Configuration for the everest-core dagger module"""

    everest_ci_version: Annotated[str, dagger.Doc("Version of the Everest CI")]
    everest_dev_environment_docker_version: Annotated[str, dagger.Doc("Version of the Everest development environment docker images")]

    source_dir: Annotated[dagger.Directory, dagger.Doc("Source directory with the source code")]

    workdir_path: Annotated[Path, dagger.Doc("Working directory path for the container")]
    source_path: Annotated[Path, dagger.Doc("Source path in the container for the source code")]
    build_path: Annotated[Path, dagger.Doc("Build directory path in the container")]
    dist_path: Annotated[Path, dagger.Doc("Dist directory path in the container")]
    wheels_path: Annotated[Path, dagger.Doc("Path in the container to install wheel files to")]
    cache_path: Annotated[Path, dagger.Doc("Cache directory path in the container")]
    artifacts_path: Annotated[Path, dagger.Doc("CI artifacts path in the container")]
