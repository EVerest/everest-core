ARG BASE_IMAGE=debian:12
FROM ${BASE_IMAGE}

ARG BASE_IMAGE
ARG USERNAME=docker
ARG USER_UID=1000

# create user and add to sudoers
RUN useradd -ms /bin/bash -u ${USER_UID} -U ${USERNAME}

# Extend the timeout
RUN mkdir -p /etc/apt/apt.conf.d/ \
    && echo 'Acquire::http::Timeout "100";' >> /etc/apt/apt.conf.d/99stahp_stahping \
    && echo 'Acquire::ftp::Timeout "100";' >> /etc/apt/apt.conf.d/99stahp_stahping \
    && echo 'Acquire::Retries "10";' >> /etc/apt/apt.conf.d/99stahp_stahping

RUN apt-get update \
    && apt-get install --no-install-recommends -y sudo \
    && echo ${USERNAME} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME} \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update \
    && apt-get install --no-install-recommends -y \
    git \
    rsync \
    wget \
    cmake \
    doxygen \
    graphviz \
    build-essential \
    clang-format \
    clang-tidy \
    cppcheck \
    libboost-all-dev \
    maven \
    openjdk-17-jdk \
    nodejs \
    npm \
    libsqlite3-dev \
    python3-pip \
    libssl-dev \
    libcurl4-openssl-dev \
    pkg-config \
    libpcap-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# create workdir and set access rights
WORKDIR /workspace/everest-cpp
RUN chown -R ${USERNAME}:${USERNAME} /workspace/everest-cpp

USER ${USERNAME}

# add github to known hosts
RUN mkdir ~/.ssh
RUN ssh-keyscan github.com > ~/.ssh/known_hosts

ENV PATH="/home/$USERNAME/.local/bin:$PATH"
ENV CPM_SOURCE_CACHE="/home/$USERNAME/cpm_source_cache"

RUN echo "echo \"To initialize the EVerest workspace please run the following command:\"" >> /home/${USERNAME}/.bashrc
RUN echo "echo \". init.sh\"" >> /home/${USERNAME}/.bashrc

CMD [ "bash" ]
