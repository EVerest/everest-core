#!/bin/bash

# Bash completion for devrd script
# Source this file or add to your .bashrc to enable completion

_devrd_completion() {
    local cur prev opts cmds
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"

    # Available commands
    cmds="install env build start stop prompt purge exec flows flow"

    # Available options
    opts="-v --version -w --workspace --help"

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

    # If the previous word is an option that takes an argument, complete based on the option
    case "$prev" in

        -v|--version)
            # Complete with common version patterns
            COMPREPLY=( $(compgen -W "main master develop release/1.0 release/1.1" -- "$cur") )
            return 0
            ;;
        -w|--workspace)
            # Complete directories
            COMPREPLY=( $(compgen -d -- "$cur") )
            return 0
            ;;
        flow)
            # Complete with available flow file paths dynamically
            local flows
            flows=$(_get_nodered_flows)
            COMPREPLY=( $(compgen -W "$flows" -- "$cur") )
            return 0
            ;;
        start|stop)
            # Complete with available container names
            local containers
            containers=$(_get_container_names)
            COMPREPLY=( $(compgen -W "$containers" -- "$cur") )
            return 0
            ;;
        exec)
            # For exec command, complete with common commands
            COMPREPLY=( $(compgen -W "ls pwd cd cmake ninja make" -- "$cur") )
            return 0
            ;;
    esac

    # If we're completing the first word (command), show commands
    if [ $COMP_CWORD -eq 1 ]; then
        COMPREPLY=( $(compgen -W "$cmds" -- "$cur") )
        return 0
    fi

    # If we're completing an option, show options
    if [[ "$cur" == -* ]]; then
        COMPREPLY=( $(compgen -W "$opts" -- "$cur") )
        return 0
    fi

    # For other cases, complete with files/directories
    COMPREPLY=( $(compgen -f -- "$cur") )
    return 0
}

# Register the completion function
complete -F _devrd_completion devrd
complete -F _devrd_completion ./devrd
complete -F _devrd_completion ../devrd