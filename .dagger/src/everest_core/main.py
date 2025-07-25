from dagger import (
    dag,
    function,
    object_type,
    Directory,
    Doc,
    File,
    DefaultPath,
    Container,
    Service,
    field,
    ReturnType,
)
import asyncio
from typing import Annotated
import traceback

from .everest_ci import (
    EverestCI,
    BaseResultType,
)

from .functions.build_kit import build_kit as BuildKit
from .functions.lint import lint as Lint
from .functions.configure_cmake_gcc import (
    configure_cmake_gcc as ConfigureCmakeGcc,
    ConfigureResult,
)
from .functions.build_cmake_gcc import (
    build_cmake_gcc as BuildCmakeGcc,
    BuildResult,
)
from .functions.unit_tests import (
    unit_tests as UnitTests,
    UnitTestsResult,
)
from .functions.install import (
    install as Install,
    InstallResult,
)
from .functions.integration_tests import (
    integration_tests as IntegrationTests,
    IntegrationTestsResult,
)
from .functions.ocpp_tests import (
    ocpp_tests as OcppTests,
    OcppTestsResult,
)

from .utils.github_status import (
    create_github_status,
)
from .utils.task_caching import (
    cached_task,
    create_key_from_container
)
from .utils.types import (
    CIWorkspaceConfig,
    GithubConfig,
    CIConfig,
)

from opentelemetry import trace


from opentelemetry import trace

from dagger import dag, function, object_type

tracer = trace.get_tracer(__name__)


@object_type
class EverestCore:
    """
    Functions that compose multiple EverestCoreFunctions and EverestCI
    functions in workflows
    """

    everest_ci_version: Annotated[
        str,
        Doc("Version of the Everest CI"),
    ] = "v1.5.3"
    everest_dev_environment_docker_version: Annotated[
        str,
        Doc(
            "Version of the Everest development environment docker images"
        ),
    ] = "docker-images-v0.1.0"

    source_dir: Annotated[
        Directory,
        Doc("Source directory with the source code"),
        DefaultPath("."),
    ]
    cache_dir: Annotated[
        Directory | None,
        Doc("Optional prefilled cache directory for the build process"),
    ] = None

    ###################################################
    # Optional overwrite options for workspace config #
    ###################################################
    overwrite_workspace_path: Annotated[
        str | None,
        Doc(
            "Overwrite the workspace path inside containers, if not provided "
            "it will be set to its default value"
        ),
    ] = None
    overwrite_source_path: Annotated[
        str | None,
        Doc(
            "Overwrite the source path inside containers, if not provided "
            "it will be set to its default value"
        ),
    ] = None
    overwrite_build_path: Annotated[
        str | None,
        Doc(
            "Overwrite the build path inside containers, if not provided "
            "it will be set to its default value"
        ),
    ] = None
    overwrite_dist_path: Annotated[
        str | None,
        Doc(
            "Overwrite the dist path inside containers, if not provided "
            "it will be set to its default value"
        ),
    ] = None
    overwrite_wheels_path: Annotated[
        str | None,
        Doc(
            "Overwrite the wheels path inside containers, if not provided "
            "it will be set to its default value"
        ),
    ] = None
    overwrite_cache_path: Annotated[
        str | None,
        Doc(
            "Overwrite the cache path inside containers, if not provided "
            "it will be set to its default value"
        ),
    ] = None
    overwrite_artifacts_path: Annotated[
        str | None,
        Doc(
            "Overwrite the artifacts path inside containers, if not provided "
            "it will be set to its default value"
        ),
    ] = None

    ##############################
    # Github Actions Environment #
    ##############################
    is_github_run: Annotated[
        bool,
        Doc("Whether the current run is a GitHub run"),
    ] = False
    github_token: Annotated[
        str | None,
        Doc("GitHub authentication token"),
    ] = None
    org_name: Annotated[
        str | None,
        Doc("GitHub organization name"),
    ] = None
    repo_name: Annotated[
        str | None,
        Doc("GitHub repository name"),
    ] = None
    sha: Annotated[
        str | None,
        Doc("Commit SHA"),
    ] = None
    run_id: Annotated[
        str | None,
        Doc("GitHub Actions run ID"),
    ] = None
    attempt_number: Annotated[
        int | None,
        Doc("GitHub Actions attempt number"),
    ] = None
    event_name: Annotated[
        str | None,
        Doc("GitHub Actions event name"),
    ] = None

    def __post_init__(self):
        key_args = {}
        if self.overwrite_workspace_path is not None:
            key_args["workspace_path"] = self.overwrite_workspace_path
        if self.overwrite_source_path is not None:
            key_args["source_path"] = self.overwrite_source_path
        if self.overwrite_build_path is not None:
            key_args["build_path"] = self.overwrite_build_path
        if self.overwrite_dist_path is not None:
            key_args["dist_path"] = self.overwrite_dist_path
        if self.overwrite_wheels_path is not None:
            key_args["wheels_path"] = self.overwrite_wheels_path
        if self.overwrite_cache_path is not None:
            key_args["cache_path"] = self.overwrite_cache_path
        if self.overwrite_artifacts_path is not None:
            key_args["artifacts_path"] = self.overwrite_artifacts_path
        self._ci_workspace_config = CIWorkspaceConfig(
            **key_args
        )

        self._github_config = GithubConfig(
            is_github_run=self.is_github_run,
            github_token=self.github_token,
            org_name=self.org_name,
            repo_name=self.repo_name,
            sha=self.sha,
            run_id=self.run_id,
            attempt_number=self.attempt_number,
            event_name=self.event_name,
        )

        if self.cache_dir is None:
            self.cache_dir = (
                dag.directory()
                .with_new_directory("CPM")
                .with_new_directory("ccache")
            )

        self._ci_config = CIConfig(
            everest_ci_version=self.everest_ci_version,
            everest_dev_environment_docker_version=(
                self.everest_dev_environment_docker_version
            ),
            source_dir=self.source_dir,
            cache_dir=self.cache_dir,
            ci_workspace_config=self._ci_workspace_config,
            github_config=self._github_config,
        )

        self._outputs = (
            dag.directory()
            .with_new_directory("artifacts")
            .with_new_directory("cache")
        )
        self._outputs_mutex = asyncio.Lock()

    @function
    def get_ci_workspace_config(self) -> CIWorkspaceConfig:
        return self._ci_workspace_config

    @function
    def get_github_config(self) -> GithubConfig:
        return self._github_config

    @function
    def get_ci_config(self) -> CIConfig:
        return self._ci_config

    @function
    @cached_task
    @create_github_status(
        condition=lambda self: self.get_github_config().is_github_run,
        auth_token=lambda self: self.get_github_config().github_token,
        org_name=lambda self: self.get_github_config().org_name,
        repo_name=lambda self: self.get_github_config().repo_name,
        sha=lambda self: self.get_github_config().sha,
        function_name="Build the build kit",
        run_id=lambda self: self.get_github_config().run_id,
        attempt_number=lambda self: self.get_github_config().attempt_number,
        description=None,
    )
    async def build_kit(self) -> EverestCI.BuildKitResult:
        """Build the everest-core build kit container"""

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.build_kit",
        ):
            return await BuildKit(
                docker_dir=self.source_dir.directory(".ci/build-kit/docker"),
                ci_config=self.get_ci_config(),
            )

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Linting the source code",
        description=None,
    )
    async def lint(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the linter in")
        ] = None,
    ) -> EverestCI.ClangFormatResult:
        """Run the linter on the source directory"""

        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(
                    f"Failed to build the build kit container: "
                    f"{res.exit_code}"
                )
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.lint",
        ):
            return await Lint(
                container=container,
                ci_config=self.get_ci_config()
            )

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Manifest Tests",
        description=None,
    )
    async def manifest_tests(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the manifest tests in")
        ] = None,
    ) -> BaseResultType:
        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(
                    f"Failed to build the build kit container: {res.exit_code}"
                )
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.manifest_tests",
        ):
            container = await (
                container
                .with_mounted_directory(
                    self.get_ci_workspace_config().source_path,
                    self.source_dir,
                )
                .with_workdir(
                    f"{self.get_ci_workspace_config().source_path}"
                )
                .with_exec(
                    [
                        "bash", "-c",
                        " ".join([
                            "python3", "-m", "pytest",
                            "tests/manifest_tests/*.py",
                        ]),
                    ],
                    expect=ReturnType.ANY,
                )
            )

            return BaseResultType(
                container=container,
                exit_code=await container.exit_code(),
            )

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="DCO Check",
        description=None,
    )
    async def dco_check(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the DCO check in")
        ] = None,
    ) -> BaseResultType:
        """Run the DCO check on the source directory"""

        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(
                    f"Failed to build the build kit container: {res.exit_code}"
                )
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.dco_check",
        ):
            if (
                self.get_github_config().is_github_run
                and self.get_github_config().event_name == "merge_group"
            ):
                return BaseResultType(
                    container=container,
                    exit_code=0,
                )
            container = await (
                container
                .with_mounted_directory(
                    self.get_ci_workspace_config().source_path,
                    self.source_dir,
                )
                .with_workdir(
                    f"{self.get_ci_workspace_config().source_path}"
                )
                .with_exec(
                    [
                        "dco-check",
                        "--default-branch", "main",
                        "-v",
                    ],
                    expect=ReturnType.ANY,
                )
            )

            return BaseResultType(
                container=container,
                exit_code=await container.exit_code(),
            )

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Bazel Build",
        description=None,
    )
    async def build_bazel(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the Bazel build in")
        ] = None,
    ) -> BaseResultType:
        """Run the Bazel build on the source directory"""

        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(
                    f"Failed to build the build kit container: {res.exit_code}"
                )
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.build_bazel",
        ):
            container = await (
                container
                .with_mounted_directory(
                    self.get_ci_workspace_config().source_path,
                    self.source_dir,
                )
                .with_workdir(
                    f"{self.get_ci_workspace_config().source_path}"
                )
                .with_exec(
                    [
                        "bazelisk", "build", "//...",
                    ],
                    expect=ReturnType.ANY,
                )
            )

            return BaseResultType(
                container=container,
                exit_code=await container.exit_code(),
            )
    
    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Bazel Test",
        description=None,
    )
    async def test_bazel(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the Bazel tests in")
        ] = None,
    ) -> BaseResultType:
        """Run the Bazel tests on the source directory"""

        if container is None:
            res = await self.build_bazel()
            if res.exit_code != 0:
                return BaseResultType(
                    container=res.container,
                    exit_code=res.exit_code,
                )
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.test_bazel",
        ):
            container = await (
                container
                .with_exec(
                    [
                        "bazelisk", "test", "//...",
                        "--test_output=errors",
                    ],
                    expect=ReturnType.ANY,
                )
            )

            return BaseResultType(
                container=container,
                exit_code=await container.exit_code(),
            )

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Configure CMake with GCC",
        description=None,
    )
    async def configure_cmake_gcc(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the function in")
        ] = None,
    ) -> ConfigureResult:
        """Configure CMake with GCC in the given container"""

        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(
                    f"Failed to build the build kit container: {res.exit_code}"
                )
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.configure_cmake_gcc",
        ):
            return await ConfigureCmakeGcc(
                container=container,
                ci_config=self.get_ci_config()
            )

    @function
    @cached_task(key_func=create_key_from_container)
    async def export_cpm_cache(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the function in")
        ] = None,
    ) -> Directory:
        """Export the cache from the configure_cmake_gcc function"""

        if container is None:
            res = await self.configure_cmake_gcc()
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.export_cpm_cache",
        ):
            async with self._outputs_mutex:
                self._outputs.with_directory(
                    "cache/CPM",
                    res.cache_cpm,
                )

            return res.cache_cpm

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Build CMake with GCC",
        description=None,
    )
    async def build_cmake_gcc(
        self,
        container: Annotated[
            Container | None,
            Doc(
                "Container to run the build in, "
                "typically the build-kit image"
            )
        ] = None,
    ) -> BuildResult:
        """
        Returns a container that builds the provided Directory inside the
        provided Container
        """

        if container is None:
            res = await self.configure_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(
                    f"Failed to configure build build-kit: {res.exit_code}"
                )
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.build_cmake_gcc",
        ):
            return await BuildCmakeGcc(
                container=container,
                ci_config=self.get_ci_config()
            )

    @function
    @cached_task(key_func=create_key_from_container)
    async def export_ccache_cache(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the function in")
        ] = None,
    ) -> Directory:
        """Export the ccache cache from the build_cmake_gcc function"""

        if container is None:
            res = await self.build_cmake_gcc()
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.export_ccache_cache",
        ):
            async with self._outputs_mutex:
                self._outputs.with_directory(
                    "cache/ccache",
                    res.cache_ccache,
                )

            return res.cache_ccache

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Run unit tests",
        description=None,
    )
    async def unit_tests(
        self,
        container: Annotated[
            Container | None,
            Doc(
                "Container to run the tests in, "
                "typically the build-kit image"
            )
        ] = None,
    ) -> UnitTestsResult:
        """
        Returns a container that run unit tests inside the provided Container
        """

        if container is None:
            res = await self.build_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build CMake: {res.exit_code}")
        container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.unit_tests",
        ):
            return await UnitTests(
                container=container,
                ci_config=self.get_ci_config()
            )

    @function
    @cached_task(key_func=create_key_from_container)
    async def export_unit_tests_log_file(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the tests in, typically the build-kit image")
        ] = None,
    ) -> File:
        """Returns the last unit tests log file"""

        if container is None:
            res = await self.unit_tests()
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.export_unit_tests_log_file",
        ):
            async with self._outputs_mutex:
                self._outputs = self._outputs.with_file(
                    "artifacts/unit-tests-log.txt",
                    res.last_test_log,
                )

            return res.last_test_log

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Install the built artifacts",
        description=None,
    )
    async def install(
        self,
        container: Annotated[
            Container | None,
            Doc(
                "Container to run the install in, "
                "typically the build-kit image"
            ),
        ] = None,
    ) -> InstallResult:
        """
        Install the built artifacts into the dist directory
        """

        if container is None:
            res = await self.build_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(
                    f"Failed to build the build kit: {res.exit_code}"
                )
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.install",
        ):
            return await Install(
                container=container,
                ci_config=self.get_ci_config()
            )

    @function
    def mqtt_server(self) -> Service:
        """Start and return mqtt server as a service"""

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.mqtt_server",
        ):
            service = (
                dag.container()
                .from_(
                    f"ghcr.io/everest/everest-dev-environment/mosquitto:"
                    f"{self.everest_dev_environment_docker_version}"
                )
                .with_exposed_port(1883)
                .with_exec([
                    "mkdir", "-p", "/etc/mosquitto",
                ])
                .with_exec([
                    "cp",
                    "/mosquitto/config/mosquitto.conf",
                    "/etc/mosquitto/mosquitto.conf",
                ])
                .as_service(
                    args=[
                        "/usr/sbin/mosquitto",
                        "-c", "/etc/mosquitto/mosquitto.conf",
                    ]
                )
            )

            return service

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Run integration tests",
        description=None,
    )
    async def integration_tests(
        self,
        container: Annotated[
            Container | None,
            Doc(
                "Container to run the integration tests in, "
                "typically the build-kit image"
            )
        ] = None,
    ) -> IntegrationTestsResult:
        """Run the integration tests"""

        if container is None:
            res = await self.install()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to install: {res.exit_code}")
            container = res.container

        mqtt_server = self.mqtt_server()

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.integration_tests",
        ):
            return await IntegrationTests(
                container=container,
                ci_config=self.get_ci_config(),
                mqtt_server=mqtt_server,
            )

    @function
    @cached_task(key_func=create_key_from_container)
    async def export_integration_tests_artifacts(
        self,
        container: Annotated[
            Container | None,
            Doc("Container to run the integration tests in, "
                "typically the build-kit image")
        ] = None,
    ) -> Directory:
        """Export the artifacts from the integration tests"""

        if container is None:
            res = await self.integration_tests()
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.export_integration_tests_artifacts",
        ):
            async with self._outputs_mutex:
                self._outputs = (
                    self._outputs.
                    with_file(
                        "artifacts/integration-tests.html",
                        res.report_html,
                    )
                    .with_file(
                        "artifacts/integration-tests.xml",
                        res.result_xml,
                    )
                )

            ret = (
                dag.directory()
                .with_file(
                    "integration-tests.html",
                    res.report_html,
                )
                .with_file(
                    "integration-tests.xml",
                    res.result_xml,
                )
            )
            return ret

    @function
    @cached_task(key_func=create_key_from_container)
    @create_github_status(
        condition=(
            lambda self, container: self.get_github_config().is_github_run
        ),
        auth_token=(
            lambda self, container: self.get_github_config().github_token
        ),
        org_name=(
            lambda self, container: self.get_github_config().org_name
        ),
        repo_name=(
            lambda self, container: self.get_github_config().repo_name
        ),
        sha=(
            lambda self, container: self.get_github_config().sha
        ),
        run_id=(
            lambda self, container: self.get_github_config().run_id
        ),
        attempt_number=(
            lambda self, container: self.get_github_config().attempt_number
        ),
        function_name="Run OCPP tests",
        description=None,
    )
    async def ocpp_tests(
        self,
        container: Annotated[
            Container | None,
            Doc(
                "Container to run the ocpp tests in, "
                "typically the build-kit image"
            )
        ] = None,
    ) -> OcppTestsResult:
        """Run the OCPP tests"""

        if container is None:
            res = await self.install()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to install: {res.exit_code}")
            container = res.container

        mqtt_server = self.mqtt_server()

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.ocpp_tests",
        ):
            return await OcppTests(
                container=container,
                ci_config=self.get_ci_config(),
                mqtt_server=mqtt_server,
            )

    @function
    @cached_task(key_func=create_key_from_container)
    async def export_ocpp_tests_artifacts(
        self,
        container: Annotated[
            Container | None,
            Doc(
                "Container to run the ocpp tests in, "
                "typically the build-kit image"
            )
        ] = None,
    ) -> Directory:
        """Export the artifacts from the OCPP tests"""

        if container is None:
            res = await self.ocpp_tests()
            container = res.container

        with tracer.start_as_current_span(
            f"{self.__class__.__name__}.export_ocpp_tests_artifacts",
        ):
            async with self._outputs_mutex:
                self._outputs = (
                    self._outputs
                    .with_file(
                        "artifacts/ocpp-tests.xml",
                        res.result_xml,
                    )
                    .with_file(
                        "artifacts/ocpp-tests.html",
                        res.report_html,
                    )
                )

            ret = (
                dag.directory().
                with_file(
                    "ocpp-tests.xml",
                    res.result_xml,
                ).with_file(
                    "ocpp-tests.html",
                    res.report_html,
                )
            )
            return ret

    @object_type
    class PullRequestResult():
        build_kit_result: EverestCI.BuildKitResult = field()
        lint_result: EverestCI.ClangFormatResult = field()
        manifest_tests_result: BaseResultType = field()
        dco_check_result: BaseResultType | None = field()
        build_bazel_result: BaseResultType = field()
        test_bazel_result: BaseResultType = field()
        configure_result: ConfigureResult = field()
        build_result: BuildResult = field()
        unit_tests_result: UnitTestsResult = field()
        install_result: InstallResult = field()
        integration_tests_result: IntegrationTestsResult = field()
        ocpp_tests_result: OcppTestsResult = field()
        exit_code: int = field()
        artifacts: Directory = field()
        cache: Directory = field()
        outputs: Directory = field()

    def _log_function_result(self, func_name: str, exit_code: int) -> bool:
        if exit_code == 0:
            print(f"✅ {func_name} succeeded")
            return True
        else:
            print(f"❌ {func_name} failed with exit code {exit_code}")
            return False

    @function
    @cached_task
    async def pull_request(self) -> PullRequestResult:
        try:
            async with asyncio.TaskGroup() as tg:
                # Pipeline steps
                build_kit_task = tg.create_task(self.build_kit())
                lint_task = tg.create_task(self.lint())
                manifest_tests_task = tg.create_task(self.manifest_tests())
                build_bazel_task = tg.create_task(self.build_bazel())
                test_bazel_task = tg.create_task(self.test_bazel())
                configure_task = tg.create_task(self.configure_cmake_gcc())
                build_task = tg.create_task(self.build_cmake_gcc())
                unit_tests_task = tg.create_task(self.unit_tests())
                install_task = tg.create_task(self.install())
                integration_tests_task = tg.create_task(self.integration_tests())
                ocpp_tests_task = tg.create_task(self.ocpp_tests())

                # Export caches
                tg.create_task(self.export_cpm_cache())
                tg.create_task(self.export_ccache_cache())

                # Export artifacts
                tg.create_task(self.export_unit_tests_log_file())
                tg.create_task(self.export_integration_tests_artifacts())
                tg.create_task(self.export_ocpp_tests_artifacts())

                run_dco_check = False
                if self.get_github_config().is_github_run:
                    if (
                        self.get_github_config().event_name == "pull_request"
                        or self.get_github_config().event_name == "merge_group"
                    ):
                        run_dco_check = True
                else:
                    run_dco_check = True
                if run_dco_check:
                    dco_check_task = tg.create_task(self.dco_check())

        except* Exception as eg:
            print("‼️ TaskGroup failed with multiple exceptions:")
            for i, exc in enumerate(eg.exceptions, 1):
                print(f"\n--- Exception {i} ---")
                traceback.print_exception(type(exc), exc, exc.__traceback__)

        success = True
        success &= self._log_function_result(
            "Build build kit",
            build_kit_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Lint",
            lint_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Manifest tests",
            manifest_tests_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Build Bazel",
            build_bazel_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Test Bazel",
            test_bazel_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Configure CMake with GCC",
            configure_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Build CMake with GCC",
            build_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Run unit tests",
            unit_tests_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Install",
            install_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Run integration tests",
            integration_tests_task.result().exit_code,
        )
        success &= self._log_function_result(
            "Run OCPP tests",
            ocpp_tests_task.result().exit_code,
        )
        if run_dco_check:
            success &= self._log_function_result(
                "DCO Check",
                dco_check_task.result().exit_code,
            )

        async with self._outputs_mutex:
            self._outputs = (
                self._outputs
                .with_new_file(
                    "success.txt",
                    "true" if success else "false"
                )
            )
            result = self.PullRequestResult(
                build_kit_result=build_kit_task.result(),
                lint_result=lint_task.result(),
                manifest_tests_result=manifest_tests_task.result(),
                dco_check_result=(
                    dco_check_task.result() if run_dco_check else None
                ),
                build_bazel_result=build_bazel_task.result(),
                test_bazel_result=test_bazel_task.result(),
                configure_result=configure_task.result(),
                build_result=build_task.result(),
                unit_tests_result=unit_tests_task.result(),
                install_result=install_task.result(),
                integration_tests_result=integration_tests_task.result(),
                ocpp_tests_result=ocpp_tests_task.result(),
                exit_code=0 if success else 1,
                artifacts=self._outputs.directory("artifacts"),
                cache=self._outputs.directory("cache"),
                outputs=self._outputs,
            )
            return result
