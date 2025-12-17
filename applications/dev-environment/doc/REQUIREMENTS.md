# Docker-Based Workspace & Tooling — Requirements Specification

Provide a lightweight, Docker-based workspace that runs both inside and outside Visual Studio Code (VS Code), orchestrates a set of containers (MQTT Explorer, OCPP/steve, Dev/Build, Node-RED), and includes a small setup tool to build, run, stop, purge, and interact with the environment. The dev container MUST support mounting a host folder as the workspace and allow building EVerest and running SIL.

---

## Scope

A minimal-dependency environment for development and simulation that:

- Works **with** VS Code (Dev Containers) and **without** VS Code (CLI).
- Runs the following containers: **MQTT Explorer**, **OCPP (steve + deps)**, **Dev/Build container**, **Node-RED for simulation**.
- Provides a **setup tool** to manage lifecycle and developer workflows.
- Supports **EVerest build** and **SIL execution** inside the dev container.

---

## Definitions

- **Dev container**: Linux container used for building/running code (incl. EVerest + SIL).
- **Setup tool**: a small CLI (Bash or Python stdlib) wrapping `docker compose` and other commands.
- **SIL**: Software-in-the-Loop execution of EVerest components.
- **Profiles**: `docker compose` profiles enabling/disabling groups of services.

---

## Target Platforms

- **MUST**: Linux x86_64
- **SHOULD**: Windows 11 (WSL2) and macOS (Apple Silicon)

---

## Functional Requirements

### F1. Minimal Dependencies

- **MUST** require only:
  1. Docker Engine (or Docker Desktop)
  2. Docker Compose V2
- **Optional** (editor integration): VS Code + “Dev Containers” extension.
- **MUST NOT** require global host package managers (Node, Python, Java, etc.).

**Acceptance:** On a clean host with only Docker/Compose, the setup tool builds and launches all profiles.

---

### F2. VS Code Integration

- Include `.devcontainer/` config enabling “Reopen in Container”.
- Dev container **MUST** expose project as `/workspace` (see F6).
- System **MUST** be usable without VS Code (see F3).

**Acceptance:** Opening the repo in VS Code and selecting “Reopen in Container” starts the dev container with mounted workspace.

---

### F3. CLI Operation (Outside VS Code)

- All features **MUST** be accessible via the setup tool without VS Code.

**Acceptance:** Every operation in F4/F5 is executable from a terminal with no editor running.

---

### F4. Supported Containers

Provide Compose services (each with its own profile):

- **mqtt-explorer** — MQTT Explorer UI container
- **ocpp-steve** — OCPP server (steve) + dependencies (e.g., DB)
- **dev** — Build/dev container (toolchain for EVerest + SIL)
- **nodered** — Node-RED for simulation
- **Zellij or TMUX** — Possibility to see the logs of the other containers (low prio for now)

**Acceptance:** `docker compose --profile <name> up` starts the corresponding service(s).

---

### F5. Setup Tool Capabilities

Two separate CLI tools **MUST** be provided:

**a) Installation Tool (`setup`)**

- Runs installation directly when executed (no command needed)

**b) Development Yard Tool (`devrd`)**

**Start/Stop**

- `start [mqtt|ocpp|dev|nodered|all]`
- `stop  [mqtt|ocpp|dev|nodered|all]`

**Build/Purge**

- `build` builds images.
- `purge` removes containers/networks and optionally images/volumes.

**Exec/Shell in Dev Container**

- `exec "<command>"` runs a shell command in the **running** dev container.
- `prompt` attaches an interactive shell to the dev container.

**Select Node-RED Flow**

- `flows` displays the available flows for the Node-RED container
- `set-flow <path-or-name>` selects the flow for the Node-RED container via bind-mount or env switch.
- Selection **MUST** persist across restarts (volume or config).

**Version checking**

- version check - the script should check the version of the docker tools it is using (docker compose). In case of major difference shall display an warning message.

**Acceptance:** Each subcommand functions as specified and returns non-zero on error.

---

### F6. Dev Container Workspace Mount

- **MUST** mount a host folder as `/workspace` (read-write).
- Host path **MUST** be configurable (default: current folder).
- File changes **MUST** be reflected both ways.

**Acceptance:** Creating a file on the host appears in `/workspace` and vice versa.

---

### F7. EVerest Build & SIL

- Dev container **MUST** include prerequisites to build EVerest.
- Dev container **COULD** include prerequisites to build an yocto image containing EVerest.

**Acceptance:** On a fresh checkout, a build of EVerest completes successfully (network access assumed for deps).

---

## Non-Functional Requirements

### N1. Simplicity

Repository **SHOULD** contain:

- `.devcontainer/docker-compose.yml` (main compose file)
- `.devcontainer/general-devcontainer/` (devcontainer configuration)
- `setup` (≤ ~100 LOC, installation only)
- `devrd` (≤ ~600 LOC, development yard management)
- `README.md` (quickstart, troubleshooting)
- Documentation files in `doc/` directory

Both tools **SHOULD NOT** depend on non-standard Python packages; if Python is used, restrict to the standard library and standard scripting. If Bash, rely only on POSIX tools.

---

### N2. Isolation & Persistence

- Use a dedicated Docker network; services **MUST** be reachable by service name.
- Persist appropriate data via named volumes (e.g., Node-RED data, steve DB).
- `devrd purge` **MUST** delete persisted data.
- Docker Compose project naming uses current folder name with `_devcontainer` suffix for consistency with VS Code.

---

### N3. Configuration

- All tunables (ports, flow path, workspace path) **MUST** be configurable via `.devcontainer/.env` and overridable via env vars/CLI flags.
- Environment configuration is auto-generated by `devrd env` command with sensible defaults.
- Workspace mapping is configurable via `devrd env -w <path>` option.

---

### N4. Observability

- `devrd start` **SHOULD** show per-service state, mapped ports, and container names.
- `devrd flows` **SHOULD** show Node-RED container status and list available flows.
- Container services summary displays actual port mappings and service URLs.

---

### N5. Security Baseline

- Containers **MUST NOT** run as root unless required; prefer an unprivileged user for dev.
- Avoid `--privileged` unless strictly necessary.
- Default ports **SHOULD** bind to localhost unless cross-host access is intended.
- SSH agent integration for Git operations with proper key management.

---

## Suggested Compose Structure (High-Level)

- **Profiles**
  - `mqtt` → MQTT Server
  - `ocpp` → MQTT Server, OCPP DB, Steve
  - `sil` → MQTT Server, Node-RED, MQTT Explorer

- **Volumes**
  - `nodered_data`, `steve_db`

- **Network**
  - Default Docker network (services reachable by name)

- **Project Naming**
  - `{workspace-folder-name}_devcontainer` (consistent with VS Code)

---

## CLI Sketch

```bash
# Installation (one-time setup)
./setup

# Development environment management
./devrd start [dev|mqtt|ocpp|nodered|all]
./devrd stop  [dev|mqtt|ocpp|nodered|all]
./devrd build [--all|service]
./devrd purge [service|all] [--with-images] [--with-volumes]
./devrd exec "<cmd>"
./devrd shell
./devrd set-flow ./flows/sim_fast.flow.json
./devrd status
./devrd logs [service] [--follow]
./devrd build-everest
./devrd run-sil [args...]
```

---

## Defaults (Editable via `.devcontainer/.env`)

```
ORGANIZATION_ARG=EVerest
REPOSITORY_HOST=github.com
REPOSITORY_USER=git
COMMIT_HASH=<auto-detected>
EVEREST_TOOL_BRANCH=main
UID=<current-user-id>
GID=<current-group-id>
HOST_WORKSPACE_FOLDER=<current-directory>
```

---

## Out-of-the-Box User Journeys

1. **Run everything in VS Code**
   `./setup` → Open repo → Reopen in Container → `./devrd start all` → open Node-RED/MQTT Explorer in browser.

2. **Run headless (no VS Code)**
   `./setup` → `./devrd start dev` → `./devrd build-everest` → `./devrd run-sil --scenario basic`

3. **Switch Node-RED flow**
   `./devrd set-flow ./flows/ac_slow.flow.json` → `./devrd stop nodered && ./devrd start nodered`

---

*If you want, I can turn this into a ready-to-use `docker-compose.yml`, `.devcontainer/devcontainer.json`, and a tiny `tool.sh` next.*

**Decision**:
The implementation will be having 3 parts:

- a bash script `setup` for the installation of the dev container
- a pure bash script `devrd` (development yard) that will handle the instrumentation of the containers (build, start, stop, prompt, etc)
- a Python script for the convenience additional command and aliases (INSIDE the devcontainer)
