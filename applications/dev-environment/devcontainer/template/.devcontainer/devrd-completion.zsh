#!/bin/zsh

# Zsh completion for devrd script
# Source this file or add to your .zshrc to enable completion

_devrd_completion() {
    local context state line
    typeset -A opt_args

    # Available commands
    local commands=(
        'env:Generate .env file with repository information'
        'build:Build the development container'
        'start:Start containers (profiles: mqtt, ocpp, sil)'
        'stop:Stop containers (profiles: mqtt, ocpp, sil)'
        'purge:Remove all devcontainer resources (containers, images, volumes)'
        'exec:Execute a command in the container'
        'prompt:Get a shell prompt in the container'
        'flows:List available flows'
        'flow:Switch to specific flow file'
    )

    # Available options
    local options=(
        '-v[Everest tool branch]:version:'
        '--version[Everest tool branch]:version:'
        '-w[Workspace directory]:directory:_files -/'
        '--workspace[Workspace directory]:directory:_files -/'
        '--help[Display help message]'
    )

    # Function to get available Node-RED flows dynamically
    _get_nodered_flows() {
        # Get the current project name (same logic as devrd script)
        local project_name="${DOCKER_COMPOSE_PROJECT_NAME:-$(basename "$(pwd)")_devcontainer}"

        # Check if we're in the right directory and container is running
        if [ -f "devrd" ] && docker compose -p "$project_name" -f .devcontainer/docker-compose.yml -f .devcontainer/general-devcontainer/docker-compose.devcontainer.yml ps devcontainer | grep -q "Up"; then
            # Get flows from the container and return full paths (relative to workspace)
            docker compose -p "$project_name" -f .devcontainer/docker-compose.yml -f .devcontainer/general-devcontainer/docker-compose.devcontainer.yml exec -T devcontainer find /workspace -name "*-flow.json" -type f 2>/dev/null | sed 's|/workspace/||' | sort
        else
            # Fallback to common flow file paths
            echo "everest-core/config/nodered/config-sil-dc-flow.json"
            echo "everest-core/config/nodered/config-sil-dc-bpt-flow.json"
            echo "everest-core/config/nodered/config-sil-energy-management-flow.json"
            echo "everest-core/config/nodered/config-sil-two-evse-flow.json"
            echo "everest-core/config/nodered/config-sil-flow.json"
        fi
    }

    # Function to get available container names
    _get_container_names() {
        echo "mqtt ocpp sil"
    }

    # Main completion logic
    _arguments -C \
        "$options[@]" \
        "1: :{_describe 'commands' commands}" \
        "*::arg:->args"

    case $state in
        args)
            case $line[1] in
                flow)
                    _values 'flow files' $(_get_nodered_flows)
                    ;;
                start|stop)
                    _values 'profiles' $(_get_container_names)
                    ;;
                exec)
                    _values 'commands' 'ls' 'pwd' 'cd' 'cmake' 'ninja' 'make'
                    ;;
                purge)
                    _files
                    ;;
            esac
            ;;
    esac
}

# Register the completion function
if command -v compdef >/dev/null 2>&1; then
    compdef _devrd_completion devrd
    compdef _devrd_completion ./devrd
    compdef _devrd_completion ../devrd
else
    echo "Warning: zsh completion system not loaded. Add 'autoload -U compinit && compinit' to your .zshrc"
fi
