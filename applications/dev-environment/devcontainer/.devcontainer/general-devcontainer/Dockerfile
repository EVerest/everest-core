# syntax=docker/dockerfile:1
FROM ghcr.io/everest/everest-ci/dev-env-base:v1.5.4

# Build arguments for customization
ARG USERNAME=docker
ARG USER_UID=1000
ARG USER_GID=1000
ARG EVEREST_TOOL_BRANCH=main
ARG ORGANIZATION_ARG=EVerest
ARG REPOSITORY_HOST=github.com
ARG REPOSITORY_USER=git

# Update the package list and install dependencies (run as root)
USER root
RUN apt-get update && apt-get install -y \
    protobuf-compiler \
    libsystemd-dev \
    tmux \
    chrpath \
    cpio \
    diffstat \
    gawk \
    wget \
    zstd \
    liblz4-tool \
    file \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y locales \
    && sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen \
    && dpkg-reconfigure --frontend=noninteractive locales  \
    && update-locale LANG=en_US.UTF-8 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

ENV LANG en_US.UTF-8


# Modify the existing docker user and group to match the host's USER_UID and USER_GID
RUN groupmod --gid ${USER_GID} ${USERNAME} && \
    usermod --uid ${USER_UID} --gid ${USER_GID} ${USERNAME} && \
    usermod -aG dialout ${USERNAME}

# Switch to the docker user
USER ${USERNAME}
WORKDIR /home/${USERNAME}

# Add known hosts for git repositories
RUN mkdir -p -m 0600 ~/.ssh && ssh-keyscan ${REPOSITORY_HOST} >> ~/.ssh/known_hosts

# EVerest Development Tool - Dependencies
RUN pip install --break-system-packages \
    nanopb

# EVerest Development Tool with cache busting based on commit hash
RUN python3 -m pip install --break-system-packages \
    git+https://github.com/EVerest/everest-dev-environment@${EVEREST_TOOL_BRANCH}#subdirectory=everest_dev_tool

# Set environment variables
ENV EVEREST_DEV_TOOL_DEFAULT_GIT_METHOD=ssh
ENV EVEREST_DEV_TOOL_DEFAULT_GIT_HOST=${REPOSITORY_HOST}
ENV EVEREST_DEV_TOOL_DEFAULT_GIT_SSH_USER=${REPOSITORY_USER}
ENV EVEREST_DEV_TOOL_DEFAULT_GIT_ORGANIZATION=${ORGANIZATION_ARG}
ENV LD_LIBRARY_PATH=/usr/local/lib

# Set up welcome message
RUN echo "echo \"🏔️ 🚘 Welcome to the EVerest development environment!\"" >> ${HOME}/.bashrc && \
    echo "echo \"To initialize the EVerest core repository everest-core in your workspace please run the following command:\"" >> ${HOME}/.bashrc && \
    echo "echo \"everest clone everest-core\"" >> ${HOME}/.bashrc && \
    echo "cd /workspace" >> ${HOME}/.bashrc

# Enable command line completion for devrd script by default
RUN echo "" >> ${HOME}/.bashrc && \
    echo "# Enable devrd command completion if available" >> ${HOME}/.bashrc && \
    echo "if [ -f /workspace/.devcontainer/devrd-completion.bash ]; then" >> ${HOME}/.bashrc && \
    echo "    source /workspace/.devcontainer/devrd-completion.bash" >> ${HOME}/.bashrc && \
    echo "fi" >> ${HOME}/.bashrc