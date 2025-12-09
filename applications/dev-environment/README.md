# EVerest Development <!-- omit in toc -->

This repository contains the steps required to build EVerest using either a container-based solution or bare metal.

- [EVerest Container Based Development](#everest-container-based-development)
  - [Prerequisites](#prerequisites)
  - [Quick Start (using VS Code Development - full automation)](#quick-start-using-vs-code-development---full-automation)
  - [Manual Docker Setup](#manual-docker-setup)
    - [Available Services and Docker Compose Profiles](#available-services-and-docker-compose-profiles)
    - [Environment Variables](#environment-variables)
    - [Working with Multiple Repositories](#working-with-multiple-repositories)
    - [Shell Completion (Optional)](#shell-completion-optional)
  - [SIL Simulation](#sil-simulation)
    - [Building EVerest](#building-everest)
    - [Available Node-RED UI Simulations](#available-node-red-ui-simulations)
    - [Troubleshooting](#troubleshooting)
- [Everest Bare Metal Development](#everest-bare-metal-development)
  - [Python Prerequisites](#python-prerequisites)
  - [EDM Prerequisites](#edm-prerequisites)
  - [Building with Tests](#building-with-tests)
  - [Cross-Compilation](#cross-compilation)

## EVerest Container Based Development

### Prerequisites

To install the prerequisites, please check your operating system or distribution online documentation:

- VS Code with Docker extension
- Docker installed (check documentation here: <https://docs.docker.com/engine/install/>)
- Docker compose installed version V2 (not working with V1). Tested with Linux, specifically with Ubuntu 22.04 and 24.04.

### Quick Start (using VS Code Development - full automation)

The easiest way to develop is using VS Code with the development container:

1. Follow the steps below
2. VS Code will automatically build the container with your repository settings
3. All development happens inside the container with the correct environment variables

The contents of your folder where the code resides (e.g., `my-workspace`) are mapped inside the container in the folder called `/workspace`.
You can exit VS Code at any time, re-running it will cause VS Code to ask you again to reopen in container.
Here are the steps required:

1. **Create a folder for your project**
    Create a new directory and navigate into it. This directory will be your new workspace or use an existing one.

    ```bash
    mkdir my-workspace
    cd my-workspace
    ```

2. **Install DevContainer template:**
   You can use the following command to download and install the devcontainer template:

   **One-liner:**

   ```bash
   curl -s https://raw.githubusercontent.com/EVerest/everest-dev-environment/main/devcontainer/template/setup-container > setup-container && chmod +x setup-container && ./setup-container
   ```

   **Manual clone (if curl fails):**

   ```bash
   git clone git@github.com:EVerest/everest-dev-environment.git
   cp everest-dev-environment/devcontainer/template/setup-container setup-container
   chmod +x setup-container
   ./setup-container
   # you can delete the everest-dev-environment folder, it is not needed anymore
   rm -rf everest-dev-environment
   ```

   The script will ask you for:
   1. **Workspace directory**: Press Enter to use current directory (recommended)
   2. **Version**: Press Enter to use 'main' (recommended)
   3. **Continue if directory not empty**: Type 'y' and press Enter (since you downloaded the setup-container script)

3. **Open in VS Code:**
  Then open the workspace in Visual Studio Code:

    ```bash
    code .
    ```

  Or press Ctrl+O to open the current folder in VSCode.

  Choose **Reopen in container** when prompted by VS Code.

### Manual Docker Setup

If you prefer to run the container outside VS Code, the `./devrd` script provides comprehensive control:

```bash
# Quick start (generate .env and start all services)
./devrd start

# Step-by-step workflow:
./devrd build                  # Build container (generates .env if missing)
./devrd start                  # Start all services (generates .env if missing)
./devrd stop                   # Stop all services
./devrd purge                  # Remove all containers, images, and volumes

# Container access:
./devrd prompt                 # Get interactive shell in container
./devrd exec <command>         # Execute single command in container

# Node-RED SIL Simulation:
./devrd flows                  # List available simulation flows
./devrd flow <path>            # Switch to specific flow file

# Custom environment configuration:
./devrd env -w /path/to/workspace  # Set workspace directory mapping
```

#### Available Services and Docker Compose Profiles

Services are organized into logical profiles for easier management:

| Profile         | Service           | Container Name                          | URL                        | Purpose                   |
| --------------- | ----------------- | --------------------------------------- | -------------------------- | ------------------------- |
| `mqtt/ocpp/sil` | **MQTT Server**   | `<prefix>_devcontainer-mqtt-server-1`   | localhost:1883             | Basic MQTT broker         |
| `ocpp`          | **OCPP DB**       | `<prefix>_devcontainer-ocpp-db-1`       | Internal                   | OCPP database             |
| `ocpp`          | **Steve (HTTP)**  | `<prefix>_devcontainer-steve-1`         | <http://localhost:8180>    | OCPP backend management   |
| `sil`           | **Node-RED UI**   | `<prefix>_devcontainer-nodered-1`       | <http://localhost:1880/ui> | SIL simulation interface  |
| `sil`           | **MQTT Explorer** | `<prefix>_devcontainer-mqtt-explorer-1` | <http://localhost:4000>    | MQTT topic browser        |
| `ocpp/sil`      | **Dev Container** | `<prefix>_devcontainer-devcontainer-1`  | Command line               | The development container |
| `ocpp/sil`      | **Docker Proxy**  | `<prefix>_devcontainer-docker-proxy-1`  | Internal                   | Secure Docker API access  |

**Note:** The `all` profile is a synthetic profile that includes all services. Use `./devrd start all` or `./devrd start` (default) to start all services.

Where `<prefix>` is the Docker Compose project name (check below).

**Usage Examples:**

```bash
# Start profiles
./devrd start                  # Start all services (generates .env if missing)
./devrd start all              # Start all services (same as above)
./devrd start sil              # Start SIL simulation tools
./devrd start ocpp             # Start OCPP backend
./devrd start mqtt             # Start only MQTT server

# Stop services
./devrd stop                   # Stop all services
./devrd stop all               # Stop all services (same as above)
./devrd stop sil               # Stop SIL profile only
./devrd stop ev-ws             # Stop all containers matching pattern 'ev-ws'
```

The Docker Compose project name determines how containers are named and grouped.
By default, it uses the **current folder name with _devcontainer suffix** (consistent with VSC behavior), but can be customized:

| Behavior     | Description                                                                                  |
| ------------ | -------------------------------------------------------------------------------------------- |
| **Default**  | Uses current folder name + `_devcontainer` (e.g., `ev-ws_devcontainer` for `/path/to/ev-ws`) |
| **Override** | Set `DOCKER_COMPOSE_PROJECT_NAME` environment variable                                       |
| **Example**  | `DOCKER_COMPOSE_PROJECT_NAME="my-project" ./devrd start`                                     |

**Container naming pattern:** `{project-name}-{service}-1`

- Default: `ev-ws_devcontainer-nodered-1`, `ev-ws_devcontainer-steve-1`
- Custom: `my-project-nodered-1`, `my-project-steve-1`

#### Environment Variables

  You can generate an .env file with auto-detected values using this command:

   ```bash
  ./devrd env                    # Generate .env file with auto-detected values
   ```

  This will create a `.devcontainer/.env` file with the following content:

   ```bash
    # Auto-generated by setup script
    ORGANIZATION_ARG=EVerest
    REPOSITORY_HOST=gitlab.com
    REPOSITORY_USER=git
    COMMIT_HASH=<..>
    EVEREST_TOOL_BRANCH=main
    UID=<..>
    GID=<..>
    HOST_WORKSPACE_FOLDER=/home/fmihut/checkout/ev-ws
   ```

These variables are automatically mapped in the container to the following environment variables:

- `ORGANIZATION_ARG`: Maps to `EVEREST_DEV_TOOL_DEFAULT_GIT_ORGANIZATION`
- `REPOSITORY_HOST`: Maps to `EVEREST_DEV_TOOL_DEFAULT_GIT_HOST`
- `REPOSITORY_USER`: Maps to `EVEREST_DEV_TOOL_DEFAULT_GIT_SSH_USER`
- `EVEREST_TOOL_BRANCH`: Maps to `EVEREST_TOOL_BRANCH`
- `HOST_WORKSPACE_FOLDER`: The directory mapped to `/workspace` inside the container

Use this mechanism if you have a different organization or git host or user.
This is useful if you have forked and you are hosting your development outside github.

**Workspace Folder Mapping:**

The `/workspace` directory inside the container can be mapped to any folder on your host system. The workspace folder is determined in the following priority order:

1. **Command line option**: `-w` or `--workspace` flag
2. **Environment variable**: `HOST_WORKSPACE_FOLDER` environment variable
3. **`.env` file**: `HOST_WORKSPACE_FOLDER` value in `.devcontainer/.env`
4. **Current directory**: Falls back to the current working directory

The `.devcontainer` directory (containing `.env` and Docker Compose files) is always located relative to where the `devrd` script is installed. This allows you to:

- Run `devrd` from any directory in your workspace
- Use a single `devrd` installation to manage multiple workspaces by changing `HOST_WORKSPACE_FOLDER` in the `.env` file

```bash
# Set workspace mapping via command line
./devrd env -w /path/to/workspace  # Map to any directory
./devrd start

# Or edit .devcontainer/.env directly and set HOST_WORKSPACE_FOLDER
# Then run from anywhere:
cd /path/to/workspace/subfolder
../devrd start  # Works correctly, uses workspace from .env file
```

#### Working with Multiple Repositories

To work with multiple everest repositories:

1. **Follow the [Quick Start](#quick-start-using-vs-code-development---full-automation) setup** to create your workspace and install the devcontainer template
2. **Start VS Code** or run the container manually
3. **Clone additional repositories:**

```bash
# Follow the Quick Start setup first and
# Build and start the environment
./devrd build # generates .env if missing and builds the container
code . # if you use VSCode
./devrd start # not using VSCode (generates .env if missing)
./devrd prompt # not using VSCode

# inside the container
cd /workspace
everest clone everest-core # or use the git command to clone
everest clone everest-framework # or use the git command to clone
cd everest-core
# Build the project (see Building EVerest section for details)
cmake -S . -B build -G Ninja
ninja -C build install
# this is building everest-core and it will use the everest-framework cloned locally
# the rest of the dependencies will be automatically downloaded by edm
# you can manually clone any of the dependencies locally if you want to
```

#### Shell Completion (Optional)

Command line completion for the `devrd` script is **enabled by default** in the container. The completion file is automatically sourced when you start a shell session.

If you want to enable completion on your host system (outside the container):

**For Bash:**

```bash
# Add to your ~/.bashrc
source .devcontainer/devrd-completion.bash
```

**For Zsh:**

```bash
# Add to your ~/.zshrc
autoload -U compinit && compinit
source .devcontainer/devrd-completion.zsh
```

**Available completions:**

- **Commands**: `env`, `build`, `start`, `stop`, `prompt`, `purge`, `exec`, `flows`, `flow`
- **Options**: `-v`, `--version`, `-w`, `--workspace`, `--help`
- **Node-RED flows**: dynamically detected from container (full file paths)
- **Directories**: for workspace option
- **Common commands**: for exec option

**Example usage:**

```bash
./devrd <TAB>                    # Shows all commands
./devrd start <TAB>              # Shows available profiles to start
```

### SIL Simulation

#### Building EVerest

```bash
# 1. Start environment (HOST)
./devrd start

# 2. Build project (CONTAINER)
./devrd prompt
cd /workspace
cmake -B build -S . -GNinja && ninja -C build install/strip

# 3. Switch to simulation (HOST)
./devrd flow everest-core/config/nodered/config-sil-dc-flow.json

# 4. Open UI
# Visit: http://localhost:1880/ui
```

**Note:** You can use `make` instead of `ninja` by removing `-G Ninja`.

#### Available Node-RED UI Simulations

| Flow File                                                            | Description          |
| -------------------------------------------------------------------- | -------------------- |
| `everest-core/config/nodered/config-sil-dc-flow.json`                | Single DC charging   |
| `everest-core/config/nodered/config-sil-dc-bpt-flow.json`            | DC charging with BPT |
| `everest-core/config/nodered/config-sil-energy-management-flow.json` | Energy management    |
| `everest-core/config/nodered/config-sil-two-evse-flow.json`          | Two EVSE simulation  |
| `everest-core/config/nodered/config-sil-flow.json`                   | Basic SIL simulation |

#### Troubleshooting

**Port conflicts:**

Each instance uses the same ports (1883, 1880, 4000, etc.). Please note that only one instance can run at a time.
If another process is using the port you need here is how to detect it and make it available:

```bash
sudo lsof -ti:1880 \| xargs sudo kill -9
./devrd start
```

**Regenerate environment configuration:**

```bash
./devrd env                   # Generate new .env file with auto-detection
```

**Customize environment variables:**

```bash
# Use specific branch for everest-dev-environment
./devrd env -v release/1.0
```

**Important:** If you manually edit the `.env` file and change `EVEREST_TOOL_BRANCH` or other build arguments, you must rebuild the container for changes to take effect:

```bash
# Option 1: Force rebuild (recommended)
./devrd env                    # Regenerate .env if you edited it manually
./devrd build                  # Rebuild with new environment variables

# Option 2: Clean rebuild (if rebuild doesn't work)
./devrd stop                   # Stop all containers, images, and volumes
./devrd purge                  # Remove all containers, images, and volumes
./devrd build                  # Rebuild from scratch
```

**Purge and rebuild:**

```bash
./devrd purge                  # Remove all resources for current folder
./devrd build                  # Will generate .env if missing
```

**Volume conflicts:**

Docker volumes are shared. Use `./devrd purge` before switching instances.

**SSH keys:**

Ensure your SSH agent has the necessary keys for all repositories.

**Container naming:**

Docker containers are named based on the workspace directory to avoid conflicts.

**Environment configuration issues:**

If you're having issues with environment variables or configuration, see the [Environment Variables](#environment-variables) section above.

**Container build issues:**

If containers fail to build or start, see the [Manual Docker Setup](#manual-docker-setup) section for basic commands.

**Switching between instances:**

```bash
# Stop current instance
./devrd stop

# Purge if switching to different branch/project
./devrd purge

# Start new instance
cd ~/different-everest-directory
./devrd start
```

## Everest Bare Metal Development

### Python Prerequisites

For development outside containers, install:

```bash
python3 -m pip install protobuf grpcio-tools nanopb==0.4.8
```

### EDM Prerequisites

To be able to compile using Forgejo, you need to have edm tool at least with version 0.8.0:

```bash
edm --version
edm 0.8.0
```

### Building with Tests

```bash
cmake -Bbuild -S. -GNinja -DBUILD_TESTING=ON -DEVEREST_EXCLUDE_MODULES="Linux_Systemd_Rauc"
ninja -C build
ninja -C build test
```

### Cross-Compilation

Install the SDK as provided by Yocto (or similar).
Activate the environment (typically by sourcing a script).

```bash
cd {...}/everest-core
cmake -S . -B build-cross -GNinja
  -DCMAKE_INSTALL_PREFIX=/var/everest
  -DEVC_ENABLE_CCACHE=1
  -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  -DEVEREST_ENABLE_JS_SUPPORT=OFF
  -DEVEREST_ENABLE_PY_SUPPORT=OFF
  -Deverest-cmake_DIR=<absolute_path_to/everest-cmake/>

DESTDIR=dist ninja -C build-cross install/strip && \
    rsync -av build-cross/dist/var/everest root@<actual_ip_address_of_target>:/var
```
