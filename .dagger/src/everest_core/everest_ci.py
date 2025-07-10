

import dagger
from dagger import dag, function, object_type
from typing import Annotated

@object_type
class BaseResultType:
    """Base class for result types"""
    container: Annotated[dagger.Container, dagger.Doc("Container that ran the operation")] = dagger.field()
    exit_code: Annotated[int, dagger.Doc("The exit code of the operation")] = dagger.field()

@object_type
class EverestCI:
    """Functions reused across the EVerest organization for CI/CD purposes"""

    source_dir_container: Annotated[str, dagger.Doc("Source directory mount point in the container")] = "/workspace/source"

    @object_type
    class ClangFormatResult(BaseResultType):
        """Result of a clang-format operation"""
        pass

    @function
    async def clang_format(self,
            container: Annotated[dagger.Container, dagger.Doc("Container to run clang-format in")],
            source_dir: Annotated[dagger.Directory, dagger.Doc("Directory to run clang-format on"), dagger.DefaultPath("/")],
            exclude: Annotated[str, dagger.Doc("comma separated list, exclude paths matching the given glob-like pattern(s)")] = "cache,build,dist",
            extensions: Annotated[str, dagger.Doc("comma separated list of file extensions")]  = "hpp,cpp",
            color: Annotated[bool, dagger.Doc("show colored diff")] = True,
            recursive: Annotated[bool, dagger.Doc("run recursively over directories")]= True,
            fix: Annotated[bool, dagger.Doc("format file instead of printing differences")] = False,
        ) -> ClangFormatResult:
        """Returns a container that run clang-format on the provided Directory inside the provided Container"""

        cmd_args = [
            "/usr/bin/run-clang-format",
            "/workspace/source",
        ]

        extensions_list = extensions.split(",")
        if len(extensions_list) == 0:
            raise ValueError("No extensions provided")
        cmd_args.extend([
            "--extensions",
            extensions,
        ])
        
        exclude_list = exclude.split(",")
        for ex in exclude_list:
            cmd_args.extend([
                "--exclude",
                f"/workspace/source/{ ex }",
            ])
        
        cmd_args.extend([
            "--color",
        ])
        if color:
            cmd_args.extend([
                "always"
            ])
        else:
            cmd_args.extend([
                "never"
            ])
        
        if recursive:
            cmd_args.extend([
                "-r",
            ])
        
        if fix:
            cmd_args.extend([
                "--in-place",
            ])

        container = await (
            container
            .with_mounted_directory(self.source_dir_container, source_dir)
            .with_workdir(self.source_dir_container)
            .with_exec(cmd_args, expect=dagger.ReturnType.ANY)
        )

        result = self.ClangFormatResult(
            container=container,
            exit_code=await container.exit_code(),
        )

        return result

    @object_type
    class BuildKitResult(BaseResultType):
        """Result of a build kit operation"""
        pass

    @function
    async def build_kit(self,
            docker_dir: Annotated[dagger.Directory, dagger.Doc("Directory of the build kit dockerfile"), dagger.DefaultPath("/.ci/build-kit/docker")],
            base_image_tag: Annotated[str, dagger.Doc("Base image tag to build the build-kit from")],
        ) -> BuildKitResult:
        """Returns a container that builds the provided Directory inside the provided Container"""
        container = await docker_dir.docker_build(
            build_args = [
                dagger.BuildArg("BASE_IMAGE_TAG", base_image_tag),
            ],
        )

        container = await (
            container
            .with_exec(
                [
                    "python3", "-m", "pip",
                    "install",
                    "--break-system-packages",
                    "pytest-html",
                ],
                expect=dagger.ReturnType.ANY,
            )
        )

        result = self.BuildKitResult(
            container=container,
            exit_code=await container.exit_code(),
        )
        return result
