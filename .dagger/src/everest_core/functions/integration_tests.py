import dagger
from dagger import object_type
from ..everest_ci import BaseResultType

@object_type
class IntegrationTestsResult(BaseResultType):
    """
    Result of the integration tests based on EverestCI.BaseResultType.
    It contains the result of the integration tests in JUnit XML format and an HTML report.

    Attributes:
    -----------
    result_xml: dagger.File
        File containing the result of the integration tests in JUnit XML format
    report_html: dagger.File
        File containing the report of the integration tests in HTML format
    """
    result_xml: dagger.File = dagger.field()
    report_html: dagger.File = dagger.field()

async def integration_tests(
    container: dagger.Container,
    source_dir: dagger.Directory,
    source_path: str,
    build_path: str,
    artifacts_path: str,
    dist_path: str,
    workdir_path: str,
    mqtt_server: dagger.Service,
) -> IntegrationTestsResult:

    # LTODO is this necessary?
    # Mount source directory
    container = await (
        container
        .with_mounted_directory(source_path, source_dir)
    )

    # Set venv from build directory
    container = await (
        container
        .with_env_variable("VIRTUAL_VENV", f"{build_path}/venv")
        .with_env_variable("PATH", "$VIRTUAL_VENV/bin:$PATH", expand=True)
    )

    # Install python packages generated from the build
    container = await (
        container
        .with_exec(
            [
                "cmake",
                "--build", build_path,
                "--target",
                    "everest-testing_pip_install_dist",
            ],
            expect=dagger.ReturnType.ANY
        )
    )
    if (await container.exit_code()) != 0:
        raise RuntimeError("Failed to install Python packages from the build process")

    # set PYTHONPATH to everestpy in dist directory
    container = await container.with_env_variable(
        "PYTHONPATH",
        f"{dist_path}/lib64/everest/everestpy:{dist_path}/lib/everest/everestpy:$PYTHONPATH",
        expand=True,
    )

    # Set service binding for the MQTT server
    container = await (
        container
        .with_service_binding("mqtt-server", mqtt_server)
        .with_env_variable("MQTT_SERVER_ADDRESS", "mqtt-server")
    )


    # Set workdir
    container = await (
        container
        .with_workdir(f"{source_path}/tests")
    )

    # Run the integration tests
    container = await (
        container
        .with_exec(
            [
                "bash", "-c",
                " ".join([
                    "python3", "-m", "pytest",
                    "-rA",
                    "--junitxml", f"{artifacts_path}/integration-tests.xml",
                    "--html", f"{artifacts_path}/integration-tests.html",
                    "--self-contained-html",
                    "core_tests/*",
                    "framework_tests/*",
                    "--everest-prefix", dist_path,
                ])
            ],
            expect=dagger.ReturnType.ANY,
        )
    )

    container = await container.with_workdir(workdir_path)

    result = IntegrationTestsResult(
        container=container,
        exit_code=await container.exit_code(),
        result_xml=container.file(f"{artifacts_path}/integration-tests.xml"),
        report_html=container.file(f"{artifacts_path}/integration-tests.html"),
    )
    return result
