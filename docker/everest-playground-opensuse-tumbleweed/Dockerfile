ARG BASE_IMAGE=opensuse/tumbleweed:latest
FROM ${BASE_IMAGE}

ARG BASE_IMAGE
ARG USERNAME=docker
ARG USER_UID=1000

# create user and add to sudoers
RUN zypper update \
    && zypper install -y sudo shadow \
    && useradd -ms /bin/bash -u ${USER_UID} -U ${USERNAME} \
    && echo ${USERNAME} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME}

RUN zypper install -y --type pattern devel_basis

RUN zypper install -y \
    git \
    rsync \
    wget \
    cmake \
    doxygen \
    graphviz \
    clang-tools \
    cppcheck \
    boost-devel \
    libboost_filesystem-devel \
    libboost_log-devel \
    libboost_program_options-devel \
    libboost_system-devel \
    libboost_thread-devel \
    maven \
    java-11-openjdk \
    java-11-openjdk-devel \
    nodejs \
    nodejs-devel \
    npm \
    python3-pip \
    gcc-c++ \
    sqlite3-devel \
    libopenssl-devel

# create workdir and set access rights
WORKDIR /workspace/everest-cpp
RUN chown -R ${USERNAME}:${USERNAME} /workspace/everest-cpp

USER ${USERNAME}

# add github to known hosts
RUN mkdir ~/.ssh
RUN ssh-keyscan github.com > ~/.ssh/known_hosts

ENV PATH="/home/$USERNAME/.local/bin:$PATH"
ENV CPM_SOURCE_CACHE="/home/$USERNAME/cpm_source_cache"

CMD [ "bash" ]
