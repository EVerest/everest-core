import asyncio
import time
import dagger
from dagger import dag, function, object_type
from functools import wraps
from typing import Annotated, Coroutine, Callable, Any, Optional

from .everest_ci import EverestCI, BaseResultType

from .functions.build_kit import build_kit as BuildKit
from .functions.lint import lint as Lint
from .functions.configure_cmake_gcc import configure_cmake_gcc as ConfigureCmakeGcc, ConfigureResult
from .functions.build_cmake_gcc import build_cmake_gcc as BuildCmakeGcc, BuildResult
from .functions.unit_tests import unit_tests as UnitTests, UnitTestsResult
from .functions.install import install as Install, InstallResult
from .functions.integration_tests import integration_tests as IntegrationTests, IntegrationTestsResult
from .functions.ocpp_tests import ocpp_tests as OcppTests, OcppTestsResult

def cached_task(
    func: Optional[Callable[..., Any]] = None,
    *,
    key_func: Optional[Callable[..., Any]] = None
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
                    if asyncio.iscoroutine(key_func):
                        key_part = await key_func(self, *args, **kwargs)
                    else:
                        key_part = key_func(self, *args, **kwargs)
                    key = f"{key}-{key_part}"
                if key not in self._cached_tasks:
                    if asyncio.iscoroutine(func):
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

    async def _create_key_from_container(self, container: dagger.Container) -> str:
        """Create a unique key for the container based on its properties."""
        return await container.id()

    @function
    @cached_task
    async def build_kit(self) -> EverestCI.BuildKitResult:
        """Build the everest-core build kit container"""

        return BuildKit(
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

    @object_type
    class PullRequestResult():
        container: dagger.Container = dagger.field()
        exit_code: int = dagger.field()
        workspace: dagger.Directory = dagger.field()
        artifacts: dagger.Directory = dagger.field()
        cache: dagger.Directory = dagger.field()
        outputs: dagger.Directory = dagger.field()

    @function
    @cached_task(key_func=_create_key_from_container)
    async def pull_request(self) -> PullRequestResult:
        artifacts = dag.directory()
        artifacts_mutex = asyncio.Lock()

        cache = dag.directory()
        cache_mutex = asyncio.Lock()

        async with asyncio.TaskGroup() as tg:
            build_kit_task = tg.create_task(self.build_kit())

            lint_task = tg.create_task(self.lint())
#
            configure_task = tg.create_task(self.configure_cmake_gcc())

            async def exp_cache_configure_task_fn():
                nonlocal configure_task, cache, cache_mutex
                res_configure = await configure_task
                async with cache_mutex:
                    cache = cache.with_directory(
                        "CPM",
                        res_configure.cache_cpm,
                    )
                    pass
            tg.create_task(exp_cache_configure_task_fn())

            build_task = tg.create_task(self.build_cmake_gcc())
            async def exp_cache_build_task_fn():
                nonlocal build_task, cache, cache_mutex
                res_build = await build_task
                async with cache_mutex:
                    cache = cache.with_directory(
                        "ccache",
                        res_build.cache_ccache,
                    )
                    pass
            tg.create_task(exp_cache_build_task_fn())

            unit_tests_task = tg.create_task(self.unit_tests())
            async def artifacts_unit_tests_task_fn():
                nonlocal unit_tests_task, artifacts, artifacts_mutex
                res_unit_tests = await unit_tests_task
                async with artifacts_mutex:
                    artifacts = artifacts.with_file(
                        "unit-tests-log.txt",
                        res_unit_tests.last_test_log,
                    )
            tg.create_task(artifacts_unit_tests_task_fn())

            install_task = tg.create_task(self.install())

            integration_tests_task = tg.create_task(self.integration_tests())
            async def artifacts_integration_tests_task_fn():
                nonlocal integration_tests_task, artifacts, artifacts_mutex
                res_integration_tests = await integration_tests_task
                async with artifacts_mutex:
                    artifacts = artifacts.with_file(
                        "integration-tests.xml",
                        res_integration_tests.result_xml,
                    ).with_file(
                        "integration-tests.html",
                        res_integration_tests.report_html,
                    )
            tg.create_task(artifacts_integration_tests_task_fn())

            ocpp_tests_task = tg.create_task(self.ocpp_tests())
            async def artifacts_ocpp_tests_task_fn():
                nonlocal ocpp_tests_task, artifacts, artifacts_mutex
                res_ocpp_tests = await ocpp_tests_task
                async with artifacts_mutex:
                    artifacts = artifacts.with_file(
                        "ocpp-tests.xml",
                        res_ocpp_tests.result_xml,
                    ).with_file(
                        "ocpp-tests.html",
                        res_ocpp_tests.report_html,
                    )
            tg.create_task(artifacts_ocpp_tests_task_fn())

        exit_code = 0
        build_kit_res = build_kit_task.result()
        if build_kit_res.exit_code == 0:
            print("✅ Build kit container built successfully")
        else:
            print("❌ Failed to build the build kit container")
            exit_code = build_kit_res.exit_code
        lint_res = lint_task.result()
        if lint_res.exit_code == 0:
            print("✅ Linting passed")
        else:
            print("❌ Linting failed")
            exit_code = lint_res.exit_code
        configure_res = configure_task.result()
        if configure_res.exit_code == 0:
            print("✅ CMake configuration succeeded")
        else:
            print("❌ CMake configuration failed")
            exit_code = configure_res.exit_code
        build_res = build_task.result()
        if build_res.exit_code == 0:
            print("✅ CMake build succeeded")
        else:
            print("❌ CMake build failed")
            exit_code = build_res.exit_code
        unit_tests_res = unit_tests_task.result()
        if unit_tests_res.exit_code == 0:
            print("✅ Unit tests passed")
        else:
            print("❌ Unit tests failed")
            exit_code = unit_tests_res.exit_code
        install_res = install_task.result()
        if install_res.exit_code == 0:
            print("✅ Installation succeeded")
        else:
            print("❌ Installation failed")
            exit_code = install_res.exit_code
        integration_tests_res = integration_tests_task.result()
        if integration_tests_res.exit_code == 0:
            print("✅ Integration tests passed")
        else:
            print("❌ Integration tests failed")
            exit_code = integration_tests_res.exit_code
        ocpp_tests_res = ocpp_tests_task.result()
        if ocpp_tests_res.exit_code == 0:
            print("✅ OCPP tests passed")
        else:
            print("❌ OCPP tests failed")
            exit_code = ocpp_tests_res.exit_code

        outputs = dag.directory()
        outputs = outputs.with_directory(
            "artifacts",
            artifacts,
        ).with_directory(
            "cache",
            cache,
        )

        result = self.PullRequestResult(
            container=unit_tests_res.container,
            exit_code=exit_code,
            workspace=unit_tests_res.container.directory(self.workdir_path),
            artifacts=artifacts,
            cache=cache,
            outputs=outputs,
        )

        return result
