# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## User Workflow Preferences

- **Plain git is allowed on non-`main` branches** for `add`, `commit`, `checkout`, `merge`, `rebase`, `stash`, `cherry-pick`. Never run write commands while `HEAD` is `main` — confirm `git rev-parse --abbrev-ref HEAD` first.
- **Never run `git push` (any branch, any flag) without an explicit one-off authorization from the user.**
- **Never post comments on GitHub PRs** — do not use `gh pr comment`, `gh pr review`, or any other mechanism to post comments on pull requests. Present code review findings in the conversation only.

## Project Overview

EVerest is an open-source modular framework for EV charging stations. `everest-core` is the main repository containing the runtime manager, all modules (EV charging logic), shared libraries, interface definitions, and test infrastructure.

The project is multi-language: C++ (primary), Python (modules and testing), Rust (some modules), JavaScript (some legacy modules). Build system is CMake/Ninja with Bazel used in CI.

## Build Commands

The build directory is `build/` relative to the repo root, with install prefix at `build/dist/`.

```bash
# Full configure (from repo root) — includes gRPC, ccache, compile warnings:
# NOTE: ccache is wired via CMAKE_*_COMPILER_LAUNCHER; without these flags, ccache is NOT used
# even if `ccache` is installed. Verify with `grep -c ccache build/build.ninja` (expect >0).
cmake -S . -B build \
  -GNinja \
  -DCMAKE_INSTALL_PREFIX=./build/dist \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_TESTING=ON \
  -DENABLE_GRPC_GENERATOR=ON \
  -DGRPC_GENERATOR_EDM=ON \
  -DGRPC_EDM=ON \
  -DISO15118_2_GENERATE_AND_INSTALL_CERTIFICATES=ON \
  -DEVEREST_ENABLE_COMPILE_WARNINGS=ON \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

# Build and install (parallel):
ninja -C build install -j$(nproc)

# Or using make (slower):
make -C build -j$(nproc) install

# Minimal configure (no gRPC/EEBUS):
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=./build/dist -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
ninja -C build install
```

## Running EVerest

```bash
./build/dist/bin/manager --config config/config-sil.yaml           # basic AC SIL
./build/dist/bin/manager --config config/config-sil-ocpp201.yaml   # + OCPP 2.0.1
./build/dist/bin/manager --config config/config-sil-dc.yaml        # DC charging
./build/dist/bin/manager --config config/config-sil-two-evse.yaml  # two EVSEs
# Log level: edit `Filter="%Severity% >= INFO"` in the deployed `build/dist/share/everest/modules/<Module>/logging.ini`
# (source template: `cmake/assets/logging.ini`). Severity values: DEBG, INFO, WARN, ERRO, CRIT.
# Per-module filter: `Filter="(%Process% contains OCPP201 and %Severity% >= DEBG)"`
```

## Testing

### Unit Tests (C++ with GTest/Catch2)

```bash
cd build && ctest --output-on-failure
cd build && ctest -R EvseManager          # filter by name substring
cd build && ctest -V -R <name>            # verbose

# Run a specific test binary directly (faster for iteration):
./build/lib/everest/framework/tests/everest-framework_tests "test name here"

# Generate code coverage report:
cmake --build build --target everest-core_create_coverage
# Report at: build/everest-core_create_coverage/index.html
```

### Integration Tests (Python/pytest)

```bash
# Prerequisites - activate venv and install test packages:
. build/venv/bin/activate
cmake --build build --target install_everest_testing

# Run core integration tests:
cd tests
pytest --everest-prefix ../build/dist core_tests/*.py framework_tests/*.py
pytest --everest-prefix ../build/dist async_api_tests/        # RpcApi async flows

# Run a single integration test:
pytest --everest-prefix ../build/dist core_tests/startup_tests.py
```

### OCPP Integration Tests

```bash
# Install test requirements (one-time):
. build/venv/bin/activate
cmake --build build --target everestpy_pip_install_dist
cmake --build build --target everest-testing_pip_install_dist
cmake --build build --target iso15118_pip_install_dist
cd tests/ocpp_tests && pip install -r requirements.txt

# Install OCPP-specific test fixtures (certs + per-EVSE custom component configs).
# tests/run-tests.sh invokes this automatically; run it manually when invoking
# pytest directly. Re-run after every `ninja install` that touches the prefix:
(cd tests/ocpp_tests/test_sets/everest-aux && \
    ./install_certs.sh ../../../../build/dist && \
    ./install_configs.sh ../../../../build/dist)

# Run an OCPP test suite (parallelism is configured in pytest.ini):
cd tests/ocpp_tests
pytest --everest-prefix ../../build/dist test_sets/ocpp201/ -v   # 2.0.1
pytest --everest-prefix ../../build/dist test_sets/ocpp16/ -v    # 1.6
pytest --everest-prefix ../../build/dist test_sets/ocpp21/ -v    # 2.1

# Run a single test file:
pytest --everest-prefix ../../build/dist test_sets/ocpp201/authorization.py -v -s
```

### EEBUS Integration Tests

```bash
. build/venv/bin/activate
cd tests/eebus_tests
pytest --everest-prefix ../../build/dist eebus_tests.py -v
```

### ISO 15118 / V2G Integration

There is no dedicated `iso15118_tests/` suite — V2G flows are exercised indirectly:
- `tests/core_tests/smoke_tests.py` — SIL end-to-end (AC + DC with ISO 15118-2)
- `tests/ocpp_tests/test_sets/validations.py` — CSMS-side ISO 15118 validation helpers
- In-module GTest: `modules/EVSE/EvseV2G/tests/` (build target, run via `ctest -R V2G`)

## Code Architecture

### Module System

EVerest is fundamentally a module framework. Each module:
- Has a `manifest.yaml` declaring what interfaces it **provides** and **requires**
- Is connected to other modules via a configuration YAML (in `config/`)
- Communicates exclusively through MQTT (via `everest-framework`)

Modules are organized under `modules/` by category:
- `EVSE/` — charging station logic: `EvseManager`, `OCPP`, `OCPP201`, `Auth`, `EvseSecurity`, `EvseV2G`, `Evse15118D20`, `EvseSlac`, `IsoMux`, `Iso15118InternetVas`, `StaticISO15118VASProvider`
- `EnergyManagement/` — `EnergyManager`, `EnergyNode`
- `EV/` — simulated EV side: `EvManager`, `EvSlac`
- `API/` — external interfaces: `API` (REST/MQTT), `RpcApi` (JSON-RPC/WebSocket), `EvAPI`
- `HardwareDrivers/` — BSPs, power meters, power supplies, isolation monitors, RFID
- `Simulation/` — SIL simulators: `YetiSimulator`, `SlacSimulator`, `DCSupplySimulator`
- `Testing/` — test doubles: `DummyTokenProvider`, `DummyTokenValidator`, etc.
- `Misc/` — utilities: `ErrorHistory`, `PersistentStore`, `System`, `Setup`, `LocalAllowlistTokenValidator`

### Framework (`lib/everest/framework/`)

The `everest-framework` library (vendored under `lib/`) provides:
- `manager` binary — reads config YAML, spawns module processes, manages lifecycle
- `ModuleAdapter` / `everest.hpp` — C++ base for module implementation
- MQTT-based IPC between modules
- Module lifecycle: `init()` → `ready()` callbacks in each `*Impl.cpp`

Generated headers live at `build/generated/modules/<ModuleName>/`. Never edit them.

**Communication patterns:**

```cpp
// Subscribe to a variable from a required interface (in init())
mod->r_evse_manager->subscribe_session_event([this](auto ev) { ... });

// Publish a variable on a provided interface
mod->p_main->publish_ready(true);

// Call a command on a required interface (only from ready() or later)
auto result = mod->r_auth->call_validate_token(token);
```

Interface definitions: `interfaces/*.yaml` | Types: `types/*.yaml` | Errors: `errors/*.yaml`

**Config YAML wiring (minimal example):**

```yaml
active_modules:
  evse_manager:
    module: EvseManager        # matches modules/<Category>/<Name>/
    config_module:
      max_current: 32
    connections:
      bsp:                     # requirement ID from manifest.yaml
        - module_id: yeti_driver
          implementation_id: board_support  # provide ID from that module's manifest
```

For module development patterns (ev-cli codegen, interface/type contracts, ev@uuid markers), see the `module-development` skill.

### Key Libraries (under `lib/everest/`)

- `framework/` — core runtime, MQTT abstraction, module loader
- `util/` — shared utilities: `fixed_vector`, `thread_pool_scaling`, `monitor`, async primitives
- `ocpp/` — libocpp (OCPP 1.6, 2.0.1, 2.1 implementation)
- `tls/` — TLS utilities for ISO 15118 / OCPP
- `slac/` — SLAC protocol (HomePlug Green PHY, used for ISO 15118 PLC)
- `iso15118/` — ISO 15118 protocol stack (V2G communication)
- `evse_security/` — PKI and certificate management

## Code Style

- **C++**: clang-format with LLVM base, 4-space indent, 120 column limit (see `.clang-format`)
- **JavaScript/YAML**: prettier with 2-space indent, single quotes (see `.prettierrc.yaml`)
- **Naming**: `PascalCase` classes, `camelCase` functions/variables, `UPPER_SNAKE_CASE` constants
- **Logging**: always `EVLOG_info / EVLOG_warning / EVLOG_error / EVLOG_debug` — never `std::cout`
- **Errors**: prefer return values / `std::optional` over exceptions for expected conditions
- **Format**: `clang-format -i <file>` | **Lint**: `run-clang-tidy -fix` (requires `-DCMAKE_RUN_CLANG_TIDY=ON`)
- Enable warnings: `-DEVEREST_ENABLE_COMPILE_WARNINGS=ON` (per-module) or `-DEVEREST_ENABLE_GLOBAL_COMPILE_WARNINGS=ON` (global)

## Important CMake Options

| Option | Default | Description |
|---|---|---|
| `BUILD_TESTING` | OFF | Enable unit + integration tests |
| `CMAKE_BUILD_TYPE` | — | Debug / Release / MinSizeRel |
| `EVEREST_ENABLE_PY_SUPPORT` | ON | Python module support |
| `EVEREST_ENABLE_RS_SUPPORT` | OFF | Rust module support |
| `EVEREST_ENABLE_JS_SUPPORT` | OFF | JavaScript module support |
| `CREATE_SYMLINKS` | OFF | Symlink JS/aux files instead of copying (dev convenience) |
| `EVEREST_EXCLUDE_MODULES` | — | Comma-separated list to skip building |
| `DISABLE_EDM` | — | Skip automatic dependency fetching (use system packages) |
| `EVEREST_LIBS_ONLY` | OFF | Only build libraries, skip modules/applications/config |
| `EVEREST_INCLUDE_LIBS` | — | Semicolon-separated allowlist of libraries to build (transitive deps auto-resolved) |
| `EVEREST_EXCLUDE_LIBS` | — | Semicolon-separated blocklist of libraries to skip |

## Skill Management

Skills live at `.claude/skills/<name>/SKILL.md` and are auto-discovered — browse that directory to see what's available. The YAML frontmatter `description` field lists trigger phrases that activate each skill.

**Create** a new skill (background Task agent) after reading 5+ files in a subsystem not covered by existing skills, when non-obvious architecture or pitfalls are learned.

**Update** an existing skill (Edit/Write directly) when a file path, function name, or description is wrong or stale.

**Skill format:** YAML frontmatter with `name`, `description` (all trigger phrases), `version: 1.0.0`. Body sections: architecture overview · key source files table · common patterns · pitfalls · testing approach. Mirror `.claude/skills/everest-energy/SKILL.md`.

## Key Module and Library Paths

| Module | Path |
|--------|------|
| EvseManager | `modules/EVSE/EvseManager/` |
| OCPP 1.6 | `modules/EVSE/OCPP/` |
| OCPP 2.0.1 / 2.1 | `modules/EVSE/OCPP201/` |
| EvseSecurity | `modules/EVSE/EvseSecurity/` |
| EvseV2G (ISO 15118-2) | `modules/EVSE/EvseV2G/` |
| Evse15118D20 (ISO 15118-20) | `modules/EVSE/Evse15118D20/` |
| Iso15118InternetVas | `modules/EVSE/Iso15118InternetVas/` |
| StaticISO15118VASProvider | `modules/EVSE/StaticISO15118VASProvider/` |
| EvseSlac | `modules/EVSE/EvseSlac/` |
| IsoMux | `modules/EVSE/IsoMux/` |
| EnergyManager | `modules/EnergyManagement/EnergyManager/` |
| EnergyNode | `modules/EnergyManagement/EnergyNode/` |
| EEBUS | `modules/EnergyManagement/EEBUS/` (only on `feature/eebus-rebased`) |
| Auth | `modules/EVSE/Auth/` |
| PersistentStore | `modules/Misc/PersistentStore/` |
| System | `modules/Misc/System/` |
| ErrorHistory | `modules/Misc/ErrorHistory/` |
| API | `modules/API/API/` |
| YetiSimulator | `modules/Simulation/YetiSimulator/` |
| DummyTokenProvider | `modules/Testing/DummyTokenProvider/` |
| DummyTokenValidator | `modules/Testing/DummyTokenValidator/` |

| Library | Path |
|---------|------|
| libocpp (OCPP 1.6 / 2.0.1 / 2.1) | `lib/everest/ocpp/` |
| evse_security (PKI/TLS) | `lib/everest/evse_security/` |
| iso15118 (V2G stack) | `lib/everest/iso15118/` |
| everest-framework (MQTT IPC) | `lib/everest/framework/` |
| slac (HomePlug PLC) | `lib/everest/slac/` |

## IDE / LSP

`compile_commands.json` lives at the repo root (copied or symlinked from `build/`). Point clangd at it with `.clangd` or:
```json
{ "compilationDatabasePath": "/home/martinlitre/Code/everest-workspace/everest-core" }
```

## Fast Build Tips

```bash
# Single module (fast iteration — no full install):
ninja -C build OCPP201

# Skip heavy modules to speed up configure/build:
cmake ... -DEVEREST_EXCLUDE_MODULES="EvseSlac;EvseV2G;IsoMux"
```

## Rules — Never Do These

- **Never run git write commands while on `main`** — verify with `git rev-parse --abbrev-ref HEAD`; on any other branch plain git (`add`, `commit`, etc.) is allowed
- **Never run `git push`** without explicit one-off authorization from the user, regardless of branch
- **Never post comments on GitHub PRs** — present findings in conversation only
- **Never remove `// ev@<uuid>:v1` markers** from CMakeLists.txt or *Impl.cpp files
- **Never edit files under `build/generated/`** — they are overwritten by the build
- **Never use `std::cout` or `fprintf` for logging** — use `EVLOG_info/warning/error/debug` macros
- **Never call `mod->r_*->call_*()` inside `init()`** — only in `ready()` or later
- **Never skip clang-format** before proposing changes to C++ files — run `clang-format -i <file>` manually (no pre-commit or editor hook is configured)

## OCPP Spec Documents (local)

```
~/Docs/OCPP/OCPP-2.1_Edition2_all_files/              # main 2.1 spec — always prefer Edition 2
  OCPP-2.1_edition2_part2_specification.pdf
  OCPP-2.1_edition2_part2_appendices_v21.pdf
  OCPP-2.1_edition2_part6-testcases.pdf

~/Docs/OCPP/OCPP-2.1_all_files/                       # legacy Edition 1 — do not use

~/Docs/OCPP/OCPP-2.0.1_all_files/
  OCPP-2.0.1_edition3_part2_specification.pdf

~/Docs/OCPP/OCPP_1.6_documentation_2019_12/
```

## Documentation Review After Changes

After meaningful code changes (new public API, behaviour change, bug fix, state machine update), dispatch a **background** Haiku agent to check if `modules/<Category>/<Module>/doc.rst` is still accurate. The agent should **report findings only** — do not auto-edit docs unless the user asks. Module docs live at `modules/<Category>/<Module>/doc.rst`.

