import asyncio
import time
import dagger
from dagger import dag, function, object_type
from functools import wraps
from typing import Annotated, Coroutine, Callable, Any, Optional
import inspect

from .everest_ci import EverestCI, BaseResultType

from .functions.build_kit import build_kit as BuildKit
from .functions.lint import lint as Lint
from .functions.configure_cmake_gcc import configure_cmake_gcc as ConfigureCmakeGcc, ConfigureResult
from .functions.build_cmake_gcc import build_cmake_gcc as BuildCmakeGcc, BuildResult
from .functions.unit_tests import unit_tests as UnitTests, UnitTestsResult
from .functions.install import install as Install, InstallResult
from .functions.integration_tests import integration_tests as IntegrationTests, IntegrationTestsResult
from .functions.ocpp_tests import ocpp_tests as OcppTests, OcppTestsResult

from .utils.github_status import initialize_status, update_status, CIStep, GithubStatusState

def cached_task(
    func: Optional[Callable[..., Any]] = None,
    *,
    key_func: Optional[Callable[..., str | None]] = None
) -> Callable[..., Coroutine[Any, Any, Any]]:
    """
    Decorator to cache and reuse asyncio Tasks for async or sync methods.

    This decorator ensures that for a given method (optionally distinguished by a key function),
    only one task is created and shared for concurrent calls, preventing redundant executions.

    Supports decorating both asynchronous (`async def`) and synchronous (`def`) functions.
    Synchronous functions are executed asynchronously using `asyncio.to_thread`.

    Parameters:
    -----------
    func : Optional[Callable[..., Awaitable or Any]]
        The function or coroutine method to decorate.
    key_func : Optional[Callable[..., Awaitable or Any]], optional
        Optional function to generate a cache key suffix based on the
        decorated function’s arguments. Can be synchronous or asynchronous.

    Returns:
    --------
    Callable[..., Coroutine]
        An async wrapper function that manages cached tasks per key.

    Usage:
    ------
    ```python
    # Example 1: Using the decorator directly on async functions without parameters
    @cached_task
    async def some_async_function(self):
        ...

    # Example 2: Using the decorator directly on sync functions without parameters
    @cached_task
    def sync_function_without_param(self):
        ...

    # Example 3: Using the decorator with a key function on async functions with parameters
    @cached_task(key_func=lambda self, param: param.id)
    async def async_function_with_param(self, param):
        ...

    # Example 4: Using the decorator with an async key function on async functions with parameters
    async def async_key_func(self, param):
        return param.id
    @cached_task(
        key_func=async_key_func
    )
    async def async_function_with_param(self, param):
        ...
    ```

    """
    def wrap_function(
        func: Callable[..., Any]
    ) -> Callable[..., Coroutine[Any, Any, Any]]:
        @wraps(func)
        async def wrapper(self: Any, *args: Any, **kwargs: Any) -> Any:
            if not hasattr(self, "_cached_tasks_lock"):
                self._cached_tasks_lock = asyncio.Lock()
            async with self._cached_tasks_lock:
                if not hasattr(self, "_cached_tasks"):
                    self._cached_tasks = {}
                key = func.__name__
                if key_func is not None:
                    if inspect.iscoroutinefunction(key_func):
                        key_part = await key_func(self, *args, **kwargs)
                    else:
                        key_part = key_func(self, *args, **kwargs)
                    if not key_part is None:
                        key = f"{key}-{key_part}"
                if key not in self._cached_tasks:
                    if inspect.iscoroutinefunction(func):
                        self._cached_tasks[key] = asyncio.create_task(func(self, *args, **kwargs))
                    else:
                        self._cached_tasks[key] = asyncio.create_task(asyncio.to_thread(func, self, *args, **kwargs))
            return await self._cached_tasks[key]
        return wrapper
    
    if func is None:
        return wrap_function
    else:
        return wrap_function(func)

@object_type
class EverestCore:
    """Functions that compose multiple EverestCoreFunctions and EverestCI functions in workflows"""

    everest_ci_version: Annotated[str, dagger.Doc("Version of the Everest CI")] = "v1.5.2"
    everest_dev_environment_docker_version: Annotated[str, dagger.Doc("Version of the Everest development environment docker images")] = "docker-images-v0.1.0"

    source_dir: Annotated[dagger.Directory, dagger.Doc("Source directory with the source code"), dagger.DefaultPath(".")]
    cache_dir: Annotated[dagger.Directory | None, dagger.Doc("Optional cache directory for the build process")] = None

    workdir_path: Annotated[str, dagger.Doc("Working directory path for the container")] = "/workspace"
    source_path: Annotated[str, dagger.Doc("Source path in the container for the source code")] = "source"
    build_path: Annotated[str, dagger.Doc("Build directory path in the container")] = "build"
    dist_path: Annotated[str, dagger.Doc("Dist directory path in the container")] = "dist"
    wheels_path: Annotated[str, dagger.Doc("Path in the container to install wheel files to")] = "wheels"
    cache_path: Annotated[str, dagger.Doc("Cache directory path in the container")] = "cache"
    artifacts_path: Annotated[str, dagger.Doc("CI artifacts path in the container")] = "artifacts"

    is_github_run: Annotated[bool, dagger.Doc("Whether the current run is a GitHub run")] = False
    github_token: Annotated[dagger.Secret | None, dagger.Doc("GitHub authentication token")] = None
    org_name: Annotated[str | None, dagger.Doc("GitHub organization name")] = None
    repo_name: Annotated[str | None, dagger.Doc("GitHub repository name")] = None
    sha: Annotated[str | None, dagger.Doc("Commit SHA")] = None

    def __post_init__(self):
        if not self.workdir_path.startswith("/"):
            print(f"Warning: workdir_path '{self.workdir_path}' does not start with '/', it will be prefixed with '/'")
            self.workdir_path = f"/{self.workdir_path}"
        if not self.source_path.startswith("/"):
            self.source_path = f"{self.workdir_path}/{self.source_path}"
        if not self.build_path.startswith("/"):
            self.build_path = f"{self.workdir_path}/{self.build_path}"
        if not self.dist_path.startswith("/"):
            self.dist_path = f"{self.workdir_path}/{self.dist_path}"
        if not self.wheels_path.startswith("/"):
            self.wheels_path = f"{self.workdir_path}/{self.wheels_path}"
        if not self.cache_path.startswith("/"):
            self.cache_path = f"{self.workdir_path}/{self.cache_path}"
        if not self.artifacts_path.startswith("/"):
            self.artifacts_path = f"{self.workdir_path}/{self.artifacts_path}"

        if self.cache_dir is None:
            self.cache_dir = (
                dag.directory()
                .with_new_directory("CPM")
                .with_new_directory("ccache")
            )
        
        self._outputs = (
            dag.directory()
            .with_new_directory("artifacts")
            .with_new_directory("cache")
        )
        self._outputs_mutex = asyncio.Lock()

    @function
    async def test_github_status(self) -> None:
        initialize_status(
            auth_token=await self.github_token.plaintext(),
            org_name=self.org_name,
            repo_name=self.repo_name,
            sha=self.sha,
            step=CIStep.BUILD_CMAKE_GCC,
        )
        # wait 20 seconds to simulate a long-running task
        time.sleep(20)
        update_status(
            auth_token=await self.github_token.plaintext(),
            org_name=self.org_name,
            repo_name=self.repo_name,
            sha=self.sha,
            step=CIStep.BUILD_CMAKE_GCC,
            state=GithubStatusState.SUCCESS,
            target_url="https://example.com",
        )

    async def _create_key_from_container(self, container: dagger.Container | None = None) -> str:
        """Create a unique key for the container based on its properties."""
        if container is None:
            return None
        return await container.id()

    @function
    @cached_task
    async def build_kit(self) -> EverestCI.BuildKitResult:
        """Build the everest-core build kit container"""

        return await BuildKit(
            docker_dir=self.source_dir.directory(".ci/build-kit/docker"),
            base_image_tag=self.everest_ci_version,
        )

    @function
    @cached_task(key_func=_create_key_from_container)
    async def lint(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the linter in")] = None,
    ) -> EverestCI.ClangFormatResult:
        """Run the linter on the source directory"""

        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build the build kit container: {res.exit_code}")
            container = res.container

        return await Lint(
            container=container,
            source_dir=self.source_dir,
        )

    @function
    @cached_task(key_func=_create_key_from_container)
    async def configure_cmake_gcc(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the function in")] = None,
    ) -> ConfigureResult:
        """Configure CMake with GCC in the given container"""

        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build the build kit container: {res.exit_code}")
            container = res.container

        return await ConfigureCmakeGcc(
            container=container,
            source_dir=self.source_dir,
            workdir_path=self.workdir_path,
            source_path=self.source_path,
            build_path=self.build_path,
            dist_path=self.dist_path,
            wheels_path=self.wheels_path,
            cache_dir=self.cache_dir,
            cache_path=self.cache_path,
        )
    
    @function
    @cached_task(key_func=_create_key_from_container)
    async def export_cpm_cache(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the function in")] = None,
    ) -> dagger.Directory:
        """Export the cache from the configure_cmake_gcc function"""

        if container is None:
            res = await self.configure_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to configure CMake with GCC: {res.exit_code}")
            container = res.container

        async with self._outputs_mutex:
            self._outputs.with_directory(
                "cache/CPM",
                res.cache_cpm,
            )
        
        return res.cache_cpm

    @function
    @cached_task(key_func=_create_key_from_container)
    async def build_cmake_gcc(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the build in, typically the build-kit image")] = None,
    ) -> BuildResult:
        """Returns a container that builds the provided Directory inside the provided Container"""

        if container is None:
            res = await self.configure_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to configure build build-kit: {res.exit_code}")
            container = res.container

        return await BuildCmakeGcc(
            container=container,
            source_dir=self.source_dir,
            source_path=self.source_path,
            build_path=self.build_path,
            workdir_path=self.workdir_path,
            cache_dir=self.cache_dir,
            cache_path=self.cache_path,
        )
    
    @function
    @cached_task(key_func=_create_key_from_container)
    async def export_ccache_cache(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the function in")] = None,
    ) -> dagger.Directory:
        """Export the ccache cache from the build_cmake_gcc function"""

        if container is None:
            res = await self.build_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build CMake with GCC: {res.exit_code}")
            container = res.container

        async with self._outputs_mutex:
            self._outputs.with_directory(
                "cache/ccache",
                res.cache_ccache,
            )
        
        return res.cache_ccache

    @function
    @cached_task(key_func=_create_key_from_container)
    async def unit_tests(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the tests in, typically the build-kit image")] = None,
    ) -> UnitTestsResult:
        """Returns a container that run unit tests inside the provided Container"""

        if container is None:
            res = await self.build_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build CMake: {res.exit_code}")
            container = res.container

        return await UnitTests(
            container=container,
            source_dir=self.source_dir,
            source_path=self.source_path,
            build_path=self.build_path,
            workdir_path=self.workdir_path,
        )

    @function
    @cached_task(key_func=_create_key_from_container)
    async def export_unit_tests_log_file(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the tests in, typically the build-kit image")] = None,
    ) -> dagger.File:
        """Returns the last unit tests log file"""

        if container is None:
            res = await self.unit_tests()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to run unit tests: {res.exit_code}")
            container = res.container

        async with self._outputs_mutex:
            self._outputs = self._outputs.with_file(
                "artifacts/unit-tests-log.txt",
                res.last_test_log,
            )

        return res.last_test_log

    @function
    @cached_task(key_func=_create_key_from_container)
    async def install(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the install in, typically the build-kit image")] = None,
    ) -> InstallResult:
        """Install the built artifacts into the dist directory"""

        if container is None:
            res = await self.build_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build the build kit: {res.exit_code}")
            container = res.container

        return await Install(
            container=container,
            source_dir=self.source_dir,
            source_path=self.source_path,
            build_path=self.build_path,
            dist_path=self.dist_path,
            workdir_path=self.workdir_path,
        )

    @function
    def mqtt_server(self) -> dagger.Service:
        """Start and return mqtt server as a service"""

        service = (
            dag.container()
            .from_(f"ghcr.io/everest/everest-dev-environment/mosquitto:{ self.everest_dev_environment_docker_version }")
            .with_exposed_port(1883)
            .with_exec([
                "mkdir", "-p", "/etc/mosquitto",
            ])
            .with_exec([
                "cp", "/mosquitto/config/mosquitto.conf", "/etc/mosquitto/mosquitto.conf",
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
    @cached_task(key_func=_create_key_from_container)
    async def integration_tests(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the integration tests in, typically the build-kit image")] = None,
    ) -> IntegrationTestsResult:
        """Run the integration tests"""

        if container is None:
            res = await self.install()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to install: {res.exit_code}")
            container = res.container

        mqtt_server = self.mqtt_server()

        return await IntegrationTests(
            container=container,
            source_dir=self.source_dir,
            source_path=self.source_path,
            build_path=self.build_path,
            artifacts_path=self.artifacts_path,
            dist_path=self.dist_path,
            workdir_path=self.workdir_path,
            mqtt_server=mqtt_server,
        )
    
    @function
    @cached_task(key_func=_create_key_from_container)
    async def export_integration_tests_artifacts(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the integration tests in, typically the build-kit image")] = None,
    ) -> dagger.Directory:
        """Export the artifacts from the integration tests"""

        if container is None:
            res = await self.integration_tests()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to run integration tests: {res.exit_code}")
            container = res.container

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
    @cached_task(key_func=_create_key_from_container)
    async def ocpp_tests(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the ocpp tests in, typically the build-kit image")] = None,
    ) -> OcppTestsResult:
        """Run the OCPP tests"""

        if container is None:
            res = await self.install()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to install: {res.exit_code}")
            container = res.container

        mqtt_server = self.mqtt_server()

        return await OcppTests(
            container=container,
            source_dir=self.source_dir,
            source_path=self.source_path,
            build_path=self.build_path,
            artifacts_path=self.artifacts_path,
            dist_path=self.dist_path,
            workdir_path=self.workdir_path,
            mqtt_server=mqtt_server,
        )

    @function
    @cached_task(key_func=_create_key_from_container)
    async def export_ocpp_tests_artifacts(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the ocpp tests in, typically the build-kit image")] = None,
    ) -> dagger.Directory:
        """Export the artifacts from the OCPP tests"""

        if container is None:
            res = await self.ocpp_tests()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to run OCPP tests: {res.exit_code}")
            container = res.container

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
        build_kit_result: EverestCI.BuildKitResult = dagger.field()
        lint_result: EverestCI.ClangFormatResult = dagger.field()
        configure_result: ConfigureResult = dagger.field()
        build_result: BuildResult = dagger.field()
        unit_tests_result: UnitTestsResult = dagger.field()
        install_result: InstallResult = dagger.field()
        integration_tests_result: IntegrationTestsResult = dagger.field()
        ocpp_tests_result: OcppTestsResult = dagger.field()
        exit_code: int = dagger.field()
        artifacts: dagger.Directory = dagger.field()
        cache: dagger.Directory = dagger.field()
        outputs: dagger.Directory = dagger.field()

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
        async with asyncio.TaskGroup() as tg:
            # Pipeline steps
            build_kit_task = tg.create_task(self.build_kit())
            lint_task = tg.create_task(self.lint())
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

        success = True
        success &= self._log_function_result("Build build kit", build_kit_task.result().exit_code)
        success &= self._log_function_result("Lint", lint_task.result().exit_code)
        success &= self._log_function_result("Configure CMake with GCC", configure_task.result().exit_code)
        success &= self._log_function_result("Build CMake with GCC", build_task.result().exit_code)
        success &= self._log_function_result("Run unit tests", unit_tests_task.result().exit_code)
        success &= self._log_function_result("Install", install_task.result().exit_code)
        success &= self._log_function_result("Run integration tests", integration_tests_task.result().exit_code)
        success &= self._log_function_result("Run OCPP tests", ocpp_tests_task.result().exit_code)

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
