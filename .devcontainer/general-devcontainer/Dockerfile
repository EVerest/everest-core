# syntax=docker/dockerfile:1
FROM ghcr.io/everest/everest-ci/dev-env-base:v1.5.4

# Build arguments for customization
# User and group configuration i.e. to match host user inside the container
ARG USERNAME=docker
ARG USER_UID=1000
ARG USER_GID=1000
# Configuration for everest-dev-tool's default git settings, can be overridden when working i.e. in a fork
ARG EVEREST_DEV_TOOL_DEFAULT_GIT_ORGANIZATION_ARG=EVerest
ARG EVEREST_DEV_TOOL_DEFAULT_GIT_HOST_ARG=github.com
ARG EVEREST_DEV_TOOL_DEFAULT_GIT_SSH_USER_ARG=git

##################################################################
# Should be moved to everest-dev-base at some point

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
    && update-locale LANG=en_US.UTF-8

ENV LANG en_US.UTF-8



# EVerest Development Tool - Dependencies
RUN pip install --break-system-packages \
    nanopb

# EVerest Development Tool

COPY --from=everest_dev_tool_src . /tmp/everest_dev_tool
RUN ls -al /tmp/everest_dev_tool \
    && python3 -m pip install \
        --break-system-packages \
        /tmp/everest_dev_tool/ \
    && rm -rf /tmp/everest_dev_tool

##################################################################

# Modify the existing docker user and group to match the host's USER_UID and USER_GID
RUN groupmod --gid ${USER_GID} ${USERNAME} && \
    usermod --uid ${USER_UID} --gid ${USER_GID} ${USERNAME} && \
    usermod -aG dialout ${USERNAME}

# Switch to the docker user
USER ${USERNAME}
WORKDIR /workspace

# Add known hosts for git repositories
RUN mkdir -p -m 0600 ~/.ssh && ssh-keyscan ${EVEREST_DEV_TOOL_DEFAULT_GIT_HOST_ARG} >> ~/.ssh/known_hosts

# Set environment variables
ENV EVEREST_DEV_TOOL_DEFAULT_GIT_METHOD=ssh
ENV EVEREST_DEV_TOOL_DEFAULT_GIT_HOST=${EVEREST_DEV_TOOL_DEFAULT_GIT_HOST_ARG}
ENV EVEREST_DEV_TOOL_DEFAULT_GIT_SSH_USER=${EVEREST_DEV_TOOL_DEFAULT_GIT_SSH_USER_ARG}
ENV EVEREST_DEV_TOOL_DEFAULT_GIT_ORGANIZATION=${EVEREST_DEV_TOOL_DEFAULT_GIT_ORGANIZATION_ARG}

# Enable command line completion for devrd script by default
RUN echo "" >> ${HOME}/.bashrc && \
    echo "# Enable devrd command completion if available" >> ${HOME}/.bashrc && \
    echo "if [ -f /workspace/applications/devrd/devrd-completion.bash ]; then" >> ${HOME}/.bashrc && \
    echo "    source /workspace/applications/devrd/devrd-completion.bash" >> ${HOME}/.bashrc && \
    echo "fi" >> ${HOME}/.bashrc

# Set up welcome message
RUN echo "echo \"🏔️ 🚘 Welcome to the EVerest development environment!\"" >> ${HOME}/.bashrc && \
    echo "echo \"Since you are working with the everest-core monorepo, you have all you need to get started here. 🎉🎉🎉\"" >> ${HOME}/.bashrc && \
    echo "echo \"You can find the devrd tool in applications/devrd/devrd\"" >> ${HOME}/.bashrc
