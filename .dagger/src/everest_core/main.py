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

@object_type
class EverestCore:
    """Functions that compose multiple EverestCoreFunctions and EverestCI functions in workflows"""

    everest_ci_version: Annotated[str, dagger.Doc("Version of the Everest CI")] = "v1.5.2"
    everest_dev_environment_docker_version: Annotated[str, dagger.Doc("Version of the Everest development environment docker images")] = "docker-images-v0.1.0"

    source_dir: Annotated[dagger.Directory, dagger.Doc("Source directory with the source code"), dagger.DefaultPath(".")]

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
            cache_path=self.cache_path,
        )

    @function
    async def build_cmake_gcc(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the build in, typically the build-kit image")] = None,
        build_dir: Annotated[dagger.Directory | None, dagger.Doc("Directory to build in")]=None,
    ) -> BuildResult:
        """Returns a container that builds the provided Directory inside the provided Container"""

        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to configure build build-kit: {res.exit_code}")
            container = res.container
        
        if build_dir is None:
            res = await self.configure_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to configure CMake: {res.exit_code}")
            build_dir = res.build_dir

        return await BuildCmakeGcc(
            container=container,
            source_dir=self.source_dir,
            source_path=self.source_path,
            build_dir=build_dir,
            build_path=self.build_path,
            workdir_path=self.workdir_path,
            cache_path=self.cache_path,
        )

    @function
    async def unit_tests(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the tests in, typically the build-kit image")] = None,
        build_dir: Annotated[dagger.Directory | None, dagger.Doc("Build directory")]=None,
    ) -> UnitTestsResult:
        """Returns a container that run unit tests inside the provided Container"""

        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build CMake: {res.exit_code}")
            container = res.container
        
        if build_dir is None:
            res = await self.build_cmake_gcc(container=container)
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build CMake: {res.exit_code}")
            build_dir = res.build_dir

        return await UnitTests(
            container=container,
            source_dir=self.source_dir,
            source_path=self.source_path,
            build_dir=build_dir,
            build_path=self.build_path,
            workdir_path=self.workdir_path,
        )

    @function
    async def install(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the install in, typically the build-kit image")] = None,
        build_dir: Annotated[dagger.Directory | None, dagger.Doc("Build directory")]=None,
    ) -> InstallResult:
        """Install the built artifacts into the dist directory"""

        if container is None:
            res = await self.build_kit()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build the build kit: {res.exit_code}")
            container = res.container
        
        if build_dir is None:
            res = await self.build_cmake_gcc(container=container)
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to build CMake: {res.exit_code}")
            build_dir = res.build_dir

        return await Install(
            container=container,
            source_dir=self.source_dir,
            source_path=self.source_path,
            build_dir=build_dir,
            build_path=self.build_path,
            dist_path=self.dist_path,
            workdir_path=self.workdir_path,
        )

    @object_type
    class BuildWheelsResult(BaseResultType):
        """Result of the build wheels"""
        wheels_dir: Annotated[dagger.Directory, dagger.Doc("Directory containing the built wheels")] = dagger.field()

    @function
    async def build_wheels(
        self,
        container: Annotated[dagger.Container | None, dagger.Doc("Container to run the build wheels in, typically the build-kit image")] = None,
        build_dir: Annotated[dagger.Directory | None, dagger.Doc("Build directory")]=None,
    ) -> BuildWheelsResult:
        """Build the wheels for the installed artifacts"""

        # LTODO use build_dir

        if container is None:
            res = await self.configure_cmake_gcc()
            if res.exit_code != 0:
                raise RuntimeError(f"Failed to configure CMake: {res.exit_code}")
            container = res.container

        container = await (
            container
            .with_mounted_directory(self.source_path, self.source_dir)
            .with_workdir(self.workdir_path)
            .with_exec(
                [
                    "cmake",
                    "--build", self.build_path,
                    "--target",
                        "everestpy_install_wheel",
                        "everest-testing_install_wheel",
                        "iso15118_install_wheel",
                ],
                expect=dagger.ReturnType.ANY
            )
        )

        result = self.BuildWheelsResult(
            container=container,
            wheels_dir=container.directory(self.wheels_path),
            exit_code=await container.exit_code(),
        )
        return result

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

    @object_type
    class IntegrationTestsResult(BaseResultType):
        """Result of the integration tests"""
        result_xml: Annotated[dagger.File, dagger.Doc("File containing the result of the integration tests in JUnit XML format")] = dagger.field()
        report_html: Annotated[dagger.File, dagger.Doc("File containing the report of the integration tests in HTML format")] = dagger.field()

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

        # Mount source directory
        container = await (
            container
            .with_mounted_directory(f"{self.workdir_path}/{self.source_path}", self.source_dir)
        )

        # Set venv from build directory
        container = await (
            container
            .with_env_variable("VIRTUAL_VENV", f"{self.workdir_path}/{self.build_path}/venv")
            .with_env_variable("PATH", "$VIRTUAL_VENV/bin:$PATH", expand=True)
        )

        # Install python packages generated from the build
        container = await (
            container
            .with_exec(
                [
                    "cmake",
                    "--build", f"{self.workdir_path}/{self.build_path}",
                    "--target",
                        "everestpy_pip_install_dist",
                        "everest-testing_pip_install_dist",
                ],
                expect=dagger.ReturnType.ANY
            )
        )
        if (await container.exit_code()) != 0:
            raise RuntimeError("Failed to install Python packages from the build process")

        # Set service binding for the MQTT server
        container = await (
            container
            .with_service_binding("mqtt-server", self.mqtt_server())
            .with_env_variable("MQTT_SERVER_ADDRESS", "mqtt-server")
        )


        # Set workdir
        container = await (
            container
            .with_workdir(f"{self.workdir_path}/{self.source_path}/tests")
        )

        # Run the integration tests
        container = await (
            container
            .with_exec(
                [
                    "ls", "-al"
                ]
            )
            .with_exec(
                [
                    "bash", "-c",
                    " ".join([
                        "python3", "-m", "pytest",
                        "-rA",
                        "--junitxml", f"{ self.workdir_path }/{self.artifacts_path}/integration-tests.xml",
                        "--html", f"{ self.workdir_path }/{self.artifacts_path}/integration-tests.html",
                        "--self-contained-html",
                        "core_tests/*",
                        "framework_tests/*",
                        "--everest-prefix", f"{self.workdir_path}/{self.dist_path}",
                    ])
                ],
                expect=dagger.ReturnType.ANY,
            )
        )

        result = self.IntegrationTestsResult(
            container=container,
            exit_code=await container.exit_code(),
            result_xml=container.file(f"{ self.workdir_path }/{self.artifacts_path}/integration-tests.xml"),
            report_html=container.file(f"{ self.workdir_path }/{self.artifacts_path}/integration-tests.html"),
        )
        return result

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
        
        # Mount source directory
        container = await (
            container
            .with_mounted_directory(f"{self.workdir_path}/{self.source_path}", self.source_dir)
        )


        # Set venv from build directory
        container = await (
            container
            .with_env_variable("VIRTUAL_VENV", f"{self.workdir_path}/{self.build_path}/venv")
            .with_env_variable("PATH", "$VIRTUAL_VENV/bin:$PATH", expand=True)
        )

        # Install python packages generated from the build
        container = await (
            container
            .with_exec(
                [
                    "cmake",
                    "--build", f"{self.workdir_path}/{self.build_path}",
                    "--target",
                        "everestpy_pip_install_dist",
                        "everest-testing_pip_install_dist",
                        "iso15118_pip_install_dist",
                ],
            )
        )
        
        # Install requirements.txt from occp tests
        container = await (
            container
            .with_exec(
                [
                    "pip",
                    "install",
                    "--break-system-packages",
                    "-r", f"{self.workdir_path}/{self.source_path}/tests/ocpp_tests/requirements.txt",
                ]
            )
        )

        # Execute install_certs.sh and install_configs.sh
        container = await (
            container
            .with_workdir(f"{self.workdir_path}/{self.source_path}/tests/ocpp_tests/test_sets/everest-aux")
            .with_exec(
                [
                    "bash", "-c",
                    " ".join([
                        "./install_certs.sh",
                        f"{self.workdir_path}/{self.dist_path}",
                    ])
                ]
            )
            .with_exec(
                [
                    "bash", "-c",
                    " ".join([
                        "./install_configs.sh",
                        f"{self.workdir_path}/{self.dist_path}",
                    ])
                ]
            )
        )

        # Set service binding for the MQTT server
        container = await (
            container
            .with_service_binding("mqtt-server", self.mqtt_server())
            .with_env_variable("MQTT_SERVER_ADDRESS", "mqtt-server")
        )

        # Set workdir for OCPP tests
        container = await (
            container
            .with_workdir(f"{self.workdir_path}/{self.source_path}/tests")
        )

        # Run the OCPP tests
        parallel_tests = await container.with_exec(["nproc"]).stdout()
        parallel_tests = int(parallel_tests.strip())
        container = await (
            container
            .with_exec(
                [
                    "bash", "-c",
                    " ".join([
                        "python3", "-m", "pytest",
                        "-rA",
                        "-d", "--tx", f"\"{parallel_tests}\"*popen//python=python3",
                        "--max-worker-restart=0",
                        "--timeout=300",
                        "--junitxml", f"{ self.workdir_path }/{self.artifacts_path}/ocpp-tests.xml",
                        "--html", f"{ self.workdir_path }/{self.artifacts_path}/ocpp-tests.html",
                        "--self-contained-html",
                        "ocpp_tests/test_sets/ocpp16/*.py",
                        "ocpp_tests/test_sets/ocpp201/*.py",
                        "--everest-prefix", f"{self.workdir_path}/{self.dist_path}",
                    ])
                ],
                expect=dagger.ReturnType.ANY,
            )
        )

        result = self.OcppTestsResult(
            container=container,
            exit_code=await container.exit_code(),
            result_xml=container.file(f"{ self.workdir_path }/{self.artifacts_path}/ocpp-tests.xml"),
            report_html=container.file(f"{ self.workdir_path }/{self.artifacts_path}/ocpp-tests.html"),
        )

        return result