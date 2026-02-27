load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:local.bzl", "new_local_repository")

# =============================================================================
# C++ Cross-Compilation Toolchains Configuration
# =============================================================================

# Resolve canonical labels so constraint references work when everest-core
# is consumed as a dependency module (where @// would point at the root
# module, not everest-core).
_GNU_CONSTRAINT = str(Label("//third-party/bazel/toolchains:gnu"))
_MUSL_CONSTRAINT = str(Label("//third-party/bazel/toolchains:musl"))

def _create_cpp_toolchains():
    """Experiment to create a glibc cross-compilation toolchain."""
    repo_names = []

    # This toolchain rougly corresponds to kirkstone. Use it as an example
    # to cross compile for phyverso.
    http_archive(
        name = "cc_toolchain_aarch64-linux-gnu-x86_64-linux",
        urls = ["https://toolchains.bootlin.com/downloads/releases/toolchains/aarch64/tarballs/aarch64--glibc--stable-2022.08-1.tar.bz2"],
        sha256 = "844df3c99508030ee9cb1152cb182500bb9816ff01968f2e18591d51d766c9e7",
        strip_prefix = "aarch64--glibc--stable-2022.08-1",
        build_file_content = """
load("@@rules_cc+//cc:defs.bzl", "cc_toolchain")
load("@bazel_tools//tools/cpp:unix_cc_toolchain_config.bzl", "cc_toolchain_config")

BIN_PREFIX = "bin/aarch64-buildroot-linux-gnu-"

REPO = "external/+toolchains+cc_toolchain_aarch64-linux-gnu-x86_64-linux"
SYSROOT = REPO + "/aarch64-buildroot-linux-gnu/sysroot"

cc_toolchain_config(
    name = "aarch64-linux-glibc_toolchain_config",
    abi_libc_version = "2.35",
    abi_version = "unknown",
    compile_flags = [
        "-nostdinc++",
        "-fPIC",
        "-mno-outline-atomics",
        "-isystem", REPO + "/aarch64-buildroot-linux-gnu/include/c++/11.3.0/",
        "-isystem", REPO + "/aarch64-buildroot-linux-gnu/include/c++/11.3.0/aarch64-buildroot-linux-gnu/",
        "-isystem", REPO + "/aarch64-buildroot-linux-gnu/sysroot/usr/include/",
        "-isystem", REPO + "/lib/gcc/aarch64-buildroot-linux-gnu/11.3.0/include/",
        "-isystem", REPO + "/lib/gcc/aarch64-buildroot-linux-gnu/11.3.0/include-fixed/",
    ],
    compiler = "gcc",
    cpu = "aarch64",
    host_system_name = "local",
    target_libc = "unknown",
    target_system_name = "aarch64-linux-glibc",
    toolchain_identifier = "aarch64-linux-glibc",
    tool_paths = {
        "ar": BIN_PREFIX + "ar",
        "cpp": BIN_PREFIX + "g++.br_real",
        "gcc": BIN_PREFIX + "gcc-11.3.0.br_real",
        "dwp": BIN_PREFIX + "dwp",
        "gcov": BIN_PREFIX + "gcov",
        "ld": BIN_PREFIX + "ld",
        "nm": BIN_PREFIX + "nm",
        "objcopy": BIN_PREFIX + "objcopy",
        "objdump": BIN_PREFIX + "objdump",
        "strip": BIN_PREFIX + "strip",
        "llvm-cov": BIN_PREFIX + "llvm-cov",
    },
    link_libs = ["-lstdc++", "-lm", "-ldl"],
)

filegroup(
    name = "all",
    srcs = glob(["**/**"]),
)

cc_toolchain(
    name = "aarch64-linux-glibc_toolchain",
    # We tell the toolchain, that for every command, all the files are needed
    # This is not ideal, but simple.
    all_files = ":all",
    ar_files = ":all",
    as_files = ":all",
    compiler_files = ":all",
    dwp_files = ":all",
    linker_files = ":all",
    objcopy_files = ":all",
    strip_files = ":all",
    toolchain_config = ":aarch64-linux-glibc_toolchain_config",
)

toolchain(
    name = "aarch64-linux-glibc_toolchain_toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:arm64",
        "{gnu_constraint}",
    ],
    toolchain = ":aarch64-linux-glibc_toolchain",
    toolchain_type = "@rules_cc//cc:toolchain_type",
)
""".replace("{gnu_constraint}", _GNU_CONSTRAINT),
    )

    repo_names.append("cc_toolchain_aarch64-linux-gnu-x86_64-linux")

    http_archive(
        name = "cc_toolchain_aarch64-linux-musl-x86_64-linux",
        urls = ["https://toolchains.bootlin.com/downloads/releases/toolchains/aarch64/tarballs/aarch64--musl--stable-2023.08-1.tar.bz2"],
        sha256 = "25767ae9ca70a76e9a71a13c6bc145532066a36d118d8f0ef14bd474784095ce",
        strip_prefix = "aarch64--musl--stable-2023.08-1",
        build_file_content = """
load("@@rules_cc+//cc:defs.bzl", "cc_toolchain")
load("@bazel_tools//tools/cpp:unix_cc_toolchain_config.bzl", "cc_toolchain_config")

BIN_PREFIX = "bin/aarch64-buildroot-linux-musl-"

REPO = "external/+toolchains+cc_toolchain_aarch64-linux-musl-x86_64-linux"
SYSROOT = REPO + "/aarch64-buildroot-linux-musl/sysroot"

cc_toolchain_config(
    name = "aarch64-linux-musl_toolchain_config",
    abi_libc_version = "1.2.4",
    abi_version = "unknown",
    compile_flags = [
        "-nostdinc++",
        "-flto", "-ffat-lto-objects",
        "-isystem", REPO + "/aarch64-buildroot-linux-musl/include/c++/12.3.0/",
        "-isystem", REPO + "/aarch64-buildroot-linux-musl/include/c++/12.3.0/aarch64-buildroot-linux-musl/",
        "-isystem", REPO + "/aarch64-buildroot-linux-musl/sysroot/usr/include/",
        "-isystem", REPO + "/lib/gcc/aarch64-buildroot-linux-musl/12.3.0/include/",
        "-isystem", REPO + "/lib/gcc/aarch64-buildroot-linux-musl/12.3.0/include-fixed/",
    ],
    compiler = "gcc",
    cpu = "aarch64",
    host_system_name = "local",
    target_libc = "unknown",
    target_system_name = "aarch64-linux-musl",
    toolchain_identifier = "aarch64-linux-musl",
    tool_paths = {
        "ar": BIN_PREFIX + "ar",
        "cpp": BIN_PREFIX + "g++.br_real",
        "gcc": BIN_PREFIX + "gcc-12.3.0.br_real",
        "dwp": BIN_PREFIX + "dwp",
        "gcov": BIN_PREFIX + "gcov",
        "ld": BIN_PREFIX + "ld",
        "nm": BIN_PREFIX + "nm",
        "objcopy": BIN_PREFIX + "objcopy",
        "objdump": BIN_PREFIX + "objdump",
        "strip": BIN_PREFIX + "strip",
        "llvm-cov": BIN_PREFIX + "llvm-cov",
    },
    link_flags = ["-Wl,--start-group", "-flto", "-static"],
    link_libs = ["-Wl,-Bstatic", "-lstdc++", "-lc", "-lgcc", "-lgcc_eh"],
)

filegroup(
    name = "all",
    srcs = glob(["**/**"]),
)

cc_toolchain(
    name = "aarch64-linux-musl_toolchain",
    # We tell the toolchain, that for every command, all the files are needed
    # This is not ideal, but simple.
    all_files = ":all",
    ar_files = ":all",
    as_files = ":all",
    compiler_files = ":all",
    dwp_files = ":all",
    linker_files = ":all",
    objcopy_files = ":all",
    strip_files = ":all",
    toolchain_config = ":aarch64-linux-musl_toolchain_config",
)

toolchain(
    name = "aarch64-linux-musl_toolchain_toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:arm64",
        "{musl_constraint}",
    ],
    toolchain = ":aarch64-linux-musl_toolchain",
    toolchain_type = "@rules_cc//cc:toolchain_type",
)
""".replace("{musl_constraint}", _MUSL_CONSTRAINT),
    )

    repo_names.append("cc_toolchain_aarch64-linux-musl-x86_64-linux")

    http_archive(
        name = "cc_toolchain_armv7-linux-gnu-x86_64-linux",
        urls = ["https://toolchains.bootlin.com/downloads/releases/toolchains/armv7-eabihf/tarballs/armv7-eabihf--glibc--stable-2022.08-1.tar.bz2"],
        sha256 = "64329b3e72350ceda65997368395a945ef83769013d82414dc5f2021c33f2d44",
        strip_prefix = "armv7-eabihf--glibc--stable-2022.08-1",
        build_file_content = """
load("@@rules_cc+//cc:defs.bzl", "cc_toolchain")
load("@bazel_tools//tools/cpp:unix_cc_toolchain_config.bzl", "cc_toolchain_config")

BIN_PREFIX = "bin/arm-buildroot-linux-gnueabihf-"

REPO = "external/+toolchains+cc_toolchain_armv7-linux-gnu-x86_64-linux"

cc_toolchain_config(
    name = "armv7-linux-gnu_toolchain_config",
    abi_libc_version = "2.35",
    abi_version = "unknown",
    compile_flags = [
        "-nostdinc++",
        "-fPIC",
        "-mthumb",
        "-mfpu=neon",
        "-mfloat-abi=hard",
        "-mcpu=cortex-a7",
        "-isystem", REPO + "/arm-buildroot-linux-gnueabihf/include/c++/11.3.0/",
        "-isystem", REPO + "/arm-buildroot-linux-gnueabihf/include/c++/11.3.0/arm-buildroot-linux-gnueabihf/",
        "-isystem", REPO + "/arm-buildroot-linux-gnueabihf/sysroot/usr/include/",
        "-isystem", REPO + "/lib/gcc/arm-buildroot-linux-gnueabihf/11.3.0/include/",
        "-isystem", REPO + "/lib/gcc/arm-buildroot-linux-gnueabihf/11.3.0/include-fixed/",
    ],
    compiler = "gcc",
    cpu = "armv7",
    host_system_name = "local",
    target_libc = "unknown",
    target_system_name = "armv7-linux-gnu",
    toolchain_identifier = "armv7-linux-gnu",
    tool_paths = {
        "ar": BIN_PREFIX + "ar",
        "cpp": BIN_PREFIX + "g++.br_real",
        "gcc": BIN_PREFIX + "gcc-11.3.0.br_real",
        "dwp": BIN_PREFIX + "dwp",
        "gcov": BIN_PREFIX + "gcov",
        "ld": BIN_PREFIX + "ld",
        "nm": BIN_PREFIX + "nm",
        "objcopy": BIN_PREFIX + "objcopy",
        "objdump": BIN_PREFIX + "objdump",
        "strip": BIN_PREFIX + "strip",
        "llvm-cov": BIN_PREFIX + "llvm-cov",
    },
    link_libs = ["-lstdc++", "-lm", "-ldl"],
)

filegroup(
    name = "all",
    srcs = glob(["**/**"]),
)

cc_toolchain(
    name = "armv7-linux-gnu_toolchain",
    all_files = ":all",
    ar_files = ":all",
    as_files = ":all",
    compiler_files = ":all",
    dwp_files = ":all",
    linker_files = ":all",
    objcopy_files = ":all",
    strip_files = ":all",
    toolchain_config = ":armv7-linux-gnu_toolchain_config",
)

toolchain(
    name = "armv7-linux-gnu_toolchain_toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:armv7",
        "{gnu_constraint}",
    ],
    toolchain = ":armv7-linux-gnu_toolchain",
    toolchain_type = "@rules_cc//cc:toolchain_type",
)
""".replace("{gnu_constraint}", _GNU_CONSTRAINT),
    )

    repo_names.append("cc_toolchain_armv7-linux-gnu-x86_64-linux")

    http_archive(
        name = "cc_toolchain_armv7-linux-musl-x86_64-linux",
        urls = ["https://toolchains.bootlin.com/downloads/releases/toolchains/armv7-eabihf/tarballs/armv7-eabihf--musl--stable-2023.08-1.tar.bz2"],
        sha256 = "4f06ed760d3b2e779f0d8aec73becd21edce9d04560d2fba53549ca8c12f51ba",
        strip_prefix = "armv7-eabihf--musl--stable-2023.08-1",
        build_file_content = """
load("@@rules_cc+//cc:defs.bzl", "cc_toolchain")
load("@bazel_tools//tools/cpp:unix_cc_toolchain_config.bzl", "cc_toolchain_config")

BIN_PREFIX = "bin/arm-buildroot-linux-musleabihf-"

REPO = "external/+toolchains+cc_toolchain_armv7-linux-musl-x86_64-linux"

cc_toolchain_config(
    name = "armv7-linux-musl_toolchain_config",
    abi_libc_version = "1.2.4",
    abi_version = "unknown",
    compile_flags = [
        "-nostdinc++",
        "-isystem", REPO + "/arm-buildroot-linux-musleabihf/include/c++/12.3.0/",
        "-isystem", REPO + "/arm-buildroot-linux-musleabihf/include/c++/12.3.0/arm-buildroot-linux-musleabihf/",
        "-isystem", REPO + "/arm-buildroot-linux-musleabihf/sysroot/usr/include/",
        "-isystem", REPO + "/lib/gcc/arm-buildroot-linux-musleabihf/12.3.0/include/",
        "-isystem", REPO + "/lib/gcc/arm-buildroot-linux-musleabihf/12.3.0/include-fixed/",
    ],
    compiler = "gcc",
    cpu = "armv7",
    host_system_name = "local",
    target_libc = "unknown",
    target_system_name = "armv7-linux-musl",
    toolchain_identifier = "armv7-linux-musl",
    tool_paths = {
        "ar": BIN_PREFIX + "ar",
        "cpp": BIN_PREFIX + "g++.br_real",
        "gcc": BIN_PREFIX + "gcc-12.3.0.br_real",
        "dwp": BIN_PREFIX + "dwp",
        "gcov": BIN_PREFIX + "gcov",
        "ld": BIN_PREFIX + "ld",
        "nm": BIN_PREFIX + "nm",
        "objcopy": BIN_PREFIX + "objcopy",
        "objdump": BIN_PREFIX + "objdump",
        "strip": BIN_PREFIX + "strip",
        "llvm-cov": BIN_PREFIX + "llvm-cov",
    },
    link_flags = ["-Wl,--start-group", "-flto", "-static"],
    link_libs = ["-Wl,-Bstatic", "-lstdc++", "-lc", "-lgcc", "-lgcc_eh"],
)

filegroup(
    name = "all",
    srcs = glob(["**/**"]),
)

cc_toolchain(
    name = "armv7-linux-musl_toolchain",
    all_files = ":all",
    ar_files = ":all",
    as_files = ":all",
    compiler_files = ":all",
    dwp_files = ":all",
    linker_files = ":all",
    objcopy_files = ":all",
    strip_files = ":all",
    toolchain_config = ":armv7-linux-musl_toolchain_config",
)

toolchain(
    name = "armv7-linux-musl_toolchain_toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:armv7",
        "{musl_constraint}",
    ],
    toolchain = ":armv7-linux-musl_toolchain",
    toolchain_type = "@rules_cc//cc:toolchain_type",
)
""".replace("{musl_constraint}", _MUSL_CONSTRAINT),
    )

    repo_names.append("cc_toolchain_armv7-linux-musl-x86_64-linux")
    return repo_names

# =============================================================================
# Module Extension
# =============================================================================

def _toolchains_impl(ctx):
    _create_cpp_toolchains()

toolchains = module_extension(
    implementation = _toolchains_impl,
)

