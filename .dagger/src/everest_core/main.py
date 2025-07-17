import asyncio
import time
import dagger
from dagger import dag, function, object_type
from typing import Annotated

from .everest_ci import EverestCI, BaseResultType

from .functions.build_kit import build_kit as BuildKit
from .functions.lint import lint as Lint
from .functions.configure_cmake_gcc import configure_cmake_gcc as ConfigureCmakeGcc, ConfigureResult
from .functions.build_cmake_gcc import build_cmake_gcc as BuildCmakeGcc, BuildResult
from .functions.unit_tests import unit_tests as UnitTests, UnitTestsResult
from .functions.install import install as Install, InstallResult
from .functions.integration_tests import integration_tests as IntegrationTests, IntegrationTestsResult
from .functions.ocpp_tests import ocpp_tests as OcppTests, OcppTestsResult

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

    @function
    async def build_kit(self) -> EverestCI.BuildKitResult:
        """Build the everest-core build kit container"""

        return await BuildKit(
            docker_dir=self.source_dir.directory(".ci/build-kit/docker"),
            base_image_tag=self.everest_ci_version,
        )

    @function
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
    async def configure_cmake_gcc(self,
            container:  Annotated[dagger.Container | None, dagger.Doc("Container to run the function in")] = None,
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

    @object_type
    class BuildWheelsResult(BaseResultType):
        """Result of the build wheels"""
        wheels_dir: Annotated[dagger.Directory, dagger.Doc("Directory containing the built wheels")] = dagger.field()

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


    @object_type
    class OcppTestsResult(IntegrationTestsResult):
        pass

    @function
    async def ocpp_tests(self,
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
    async def pull_request(self) -> PullRequestResult:
        artifacts = dag.directory()
        artifacts_mutex = asyncio.Lock()

        cache = dag.directory()
        cache_mutex = asyncio.Lock()

        async with asyncio.TaskGroup() as tg:
            build_kit_task = tg.create_task(self.build_kit())
            # async def artifacts_build_kit_task_fn():
            #     nonlocal build_kit_task, artifacts, artifacts_mutex
            #     res_build_kit = await build_kit_task
            #     async with artifacts_mutex:
            #         artifacts = artifacts.with_file(
            #             "build-kit-image.tar.gz",
            #             await res_build_kit.container.as_tarball(),
            #         )
            # tg.create_task(artifacts_build_kit_task_fn())

            async def lint_task_fn() -> EverestCI.ClangFormatResult:
                nonlocal build_kit_task
                res_build_kit = await build_kit_task
                return await self.lint(container=res_build_kit.container)
            lint_task = tg.create_task(lint_task_fn())

            async def configure_task_fn() -> ConfigureResult:
                nonlocal build_kit_task
                res_build_kit = await build_kit_task
                return await self.configure_cmake_gcc(container=res_build_kit.container)
            configure_task = tg.create_task(configure_task_fn())
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

            async def build_task_fn() -> BuildResult:
                nonlocal configure_task
                res_configure = await configure_task
                return await self.build_cmake_gcc(
                    container=res_configure.container,
                )
            build_task = tg.create_task(build_task_fn())
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

            async def unit_tests_task_fn() -> UnitTestsResult:
                nonlocal build_task
                res_build = await build_task
                return await self.unit_tests(
                    container=res_build.container,
                )
            unit_tests_task = tg.create_task(unit_tests_task_fn())
            async def artifacts_unit_tests_task_fn():
                nonlocal unit_tests_task, artifacts, artifacts_mutex
                res_unit_tests = await unit_tests_task
                async with artifacts_mutex:
                    artifacts = artifacts.with_file(
                        "unit-tests-log.txt",
                        res_unit_tests.last_test_log,
                    )
            tg.create_task(artifacts_unit_tests_task_fn())

            async def install_task_fn() -> InstallResult:
                nonlocal build_task
                res_build = await build_task
                return await self.install(
                    container=res_build.container,
                )
            install_task = tg.create_task(install_task_fn())

            async def integration_tests_task_fn() -> IntegrationTestsResult:
                nonlocal install_task
                res_install = await install_task
                return await self.integration_tests(
                    container=res_install.container,
                )
            integration_tests_task = tg.create_task(integration_tests_task_fn())
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

            async def ocpp_tests_task_fn() -> OcppTestsResult:
                nonlocal install_task
                res_install = await install_task
                return await self.ocpp_tests(
                    container=res_install.container,
                )
            ocpp_tests_task = tg.create_task(ocpp_tests_task_fn())
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
