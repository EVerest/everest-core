ARG BASE_IMAGE=ubuntu
ARG BASE_IMAGE_VERSION=22.04
FROM ${BASE_IMAGE}:${BASE_IMAGE_VERSION}

ARG USERNAME=docker
ARG USER_UID=1000

# configure timezone data
ENV TIMEZONE=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TIMEZONE /etc/localtime && echo $TIMEZONE > /etc/timezone

# create user and add to sudoers
RUN useradd -ms /bin/bash -u ${USER_UID} -U ${USERNAME}

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
    libsqlite3-dev \
    python3-pip \
    libssl-dev \
    gpg \
    libcurl4-openssl-dev \
    pkg-config \
    libpcap-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# install a modern version of Node.js
ENV NODE_VERSION=node_16.x
ENV KEYRING=/usr/share/keyrings/nodesource.gpg
RUN wget --quiet -O - https://deb.nodesource.com/gpgkey/nodesource.gpg.key | gpg --dearmor | tee "${KEYRING}" >/dev/null
RUN gpg --no-default-keyring --keyring "${KEYRING}" --list-keys 9FD3B784BC1C6FC31A8A0A1C1655A0AB68576280
RUN echo "deb [signed-by=${KEYRING}] https://deb.nodesource.com/${NODE_VERSION} focal main" | tee /etc/apt/sources.list.d/nodesource.list
RUN echo "deb-src [signed-by=${KEYRING}] https://deb.nodesource.com/${NODE_VERSION} focal main" | tee -a /etc/apt/sources.list.d/nodesource.list
RUN apt-get update \
    && apt-get install --no-install-recommends -y \
    nodejs \
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
