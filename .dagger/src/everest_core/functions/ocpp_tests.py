import asyncio
import dagger
from dagger import object_type, dag
from ..everest_ci import BaseResultType
from .integration_tests import IntegrationTestsResult
from ..utils.types import CIConfig


@object_type
class OcppTestsResult(IntegrationTestsResult):
    pass


async def ocpp_tests(
    container: dagger.Container,
    ci_config: CIConfig,
    mqtt_server: dagger.Service,
) -> OcppTestsResult:

    # LTODO: Is this necessary?
    # Mount source directory
    container = await (
        container
        .with_mounted_directory(ci_config.ci_workspace_config.source_path, ci_config.source_dir)
    )

    # Set venv from build directory
    container = await (
        container
        .with_env_variable("VIRTUAL_VENV", f"{ci_config.ci_workspace_config.build_path}/venv")
        .with_env_variable("PATH", "$VIRTUAL_VENV/bin:$PATH", expand=True)
    )

    # Install python packages generated from the build
    container = await (
        container
        .with_exec(
            [
                "cmake",
                "--build", ci_config.ci_workspace_config.build_path,
                "--target",
                "everest-testing_pip_install_dist",
                "iso15118_pip_install_dist",
                "iso15118_requirements_pip_install_dist",
            ],
        )
    )

    # set PYTHONPATH to everestpy in dist directory
    container = await container.with_env_variable(
        "PYTHONPATH",
        f"{ci_config.ci_workspace_config.dist_path}/lib64/everest/everestpy:{ci_config.ci_workspace_config.dist_path}/lib/everest/everestpy:$PYTHONPATH",
        expand=True,
    )

    # Install requirements.txt from occp tests
    container = await (
        container
        .with_exec(
            [
                "pip",
                "install",
                "--break-system-packages",
                "-r", f"{ci_config.ci_workspace_config.source_path}/tests/ocpp_tests/requirements.txt",
            ]
        )
    )

    # Execute install_certs.sh and install_configs.sh
    container = await (
        container
        .with_workdir(f"{ci_config.ci_workspace_config.source_path}/tests/ocpp_tests/test_sets/everest-aux")
        .with_exec(
            [
                "bash", "-c",
                " ".join([
                    "./install_certs.sh",
                    f"{ci_config.ci_workspace_config.dist_path}",
                ])
            ]
        )
        .with_exec(
            [
                "bash", "-c",
                " ".join([
                    "./install_configs.sh",
                    f"{ci_config.ci_workspace_config.dist_path}",
                ])
            ]
        )
    )

    # Set service binding for the MQTT server
    container = await (
        container
        .with_service_binding("mqtt-server", mqtt_server)
        .with_env_variable("MQTT_SERVER_ADDRESS", "mqtt-server")
    )

    # Set workdir for OCPP tests
    container = await (
        container
        .with_workdir(f"{ci_config.ci_workspace_config.source_path}/tests")
    )

    # Run the OCPP tests
    parallel_tests = await container.with_exec(["nproc"]).stdout()
    parallel_tests = int(parallel_tests.strip())
    exit_code = 0
    try:
        async with asyncio.timeout(None):
            container = await (
                container
                .with_exec(
                    [
                        "bash", "-c",
                        " ".join([
                            "python3", "-m", "pytest",
                            "-rfE",
                            "-n", f"\"{parallel_tests}\"",
                            "--dist=loadgroup",
                            "--max-worker-restart=0",
                            "--timeout=300",
                            "--junitxml", f"{ci_config.ci_workspace_config.artifacts_path}/ocpp-tests.xml",
                            "--html", f"{ci_config.ci_workspace_config.artifacts_path}/ocpp-tests.html",
                            "--self-contained-html",
                            "ocpp_tests/test_sets/ocpp16/*.py",
                            "ocpp_tests/test_sets/ocpp201/*.py",
                            "ocpp_tests/test_sets/ocpp21/*.py",
                            "--everest-prefix", f"{ci_config.ci_workspace_config.dist_path}",
                        ])
                    ],
                    expect=dagger.ReturnType.ANY,
                )
            )
        exit_code = await container.exit_code()
    except TimeoutError:
        print("OCPP tests timed out after 15 minutes. Continuing...")
        exit_code = 1

    container = await container.with_workdir(ci_config.ci_workspace_config.workspace_path)

    result = OcppTestsResult(
        container=container,
        exit_code=exit_code,
        result_xml=container.file(f"{ci_config.ci_workspace_config.artifacts_path}/ocpp-tests.xml"),
        report_html=container.file(f"{ci_config.ci_workspace_config.artifacts_path}/ocpp-tests.html"),
    )

    return result
