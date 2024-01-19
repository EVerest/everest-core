# syntax=docker/dockerfile:1
FROM debian:11 AS builder

ARG REPO
ARG BRANCH
ARG EVEREST_CONFIG
ARG OCPP_CONFIG

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
    openjdk-11-jdk \
    nodejs \
    npm \
    libsqlite3-dev \
    python3-pip \
    libssl-dev \
    libcurl4-openssl-dev \
    libpcap-dev \
    libcap-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace/everest

# to avoid caching
ARG BUILD_DATE=Unknown 

# add github to known hosts
RUN mkdir ~/.ssh
RUN ssh-keyscan github.com > ~/.ssh/known_hosts

RUN mkdir -p /workspace/everest/cpm_source_cache    
ENV CPM_SOURCE_CACHE="/workspace/everest/cpm_source_cache"

RUN git clone https://github.com/EVerest/everest-cmake.git
RUN git clone https://github.com/EVerest/everest-utils.git
WORKDIR /workspace/everest/everest-utils/ev-dev-tools
RUN python3 -m pip install .
WORKDIR /workspace/everest
RUN git clone https://github.com/EVerest/everest-dev-environment.git
WORKDIR /workspace/everest/everest-dev-environment/dependency_manager
RUN python3 -m pip install .
WORKDIR /workspace/everest
RUN git clone https://github.com/EVerest/ext-switchev-iso15118.git

WORKDIR /workspace/everest

RUN rm -rf "/workspace/everest/$(basename "${REPO}" .git)"
RUN --mount=type=ssh git clone ${REPO}

RUN rm -rf "/workspace/everest/$(basename "${REPO}" .git)/build" && \
    cd "/workspace/everest/$(basename "${REPO}" .git)" && \
    git checkout "${BRANCH}" && \
    mkdir "/workspace/everest/$(basename "${REPO}" .git)/build" && \
    cd "/workspace/everest/$(basename "${REPO}" .git)/build" && \
    cmake .. -DEVEREST_BUILD_ALL_MODULES=ON -DCMAKE_INSTALL_PREFIX=/opt/everest && \
    make -j"$(nproc)" install

RUN mkdir -p /opt/everest/config/user-config
COPY "${EVEREST_CONFIG}" /opt/everest/config/
COPY "${OCPP_CONFIG}" /opt/everest/config/
RUN if [ "${EVEREST_CONFIG}" != "config.yaml" ]; then mv /opt/everest/config/"${EVEREST_CONFIG}" /opt/everest/config/config.yaml ; fi
RUN if [ "${OCPP_CONFIG}" != "ocpp-config.json" ]; then mv /opt/everest/config/"${OCPP_CONFIG}" /opt/everest/config/ocpp-config.json ; fi

COPY logging.ini /opt/everest/config

# syntax=docker/dockerfile:1
FROM debian:11-slim

RUN apt-get update \
    && apt-get install --no-install-recommends -y \
    openjdk-11-jre \
    nodejs \
    npm \
    python3-pip \
    sqlite3 \
    libboost-program-options1.74.0 \
    libboost-log1.74.0 \
    libboost-chrono1.74.0 \
    libboost-system1.74.0 \
    libssl1.1 \
    libcurl4 \
    less \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /workspace/everest/ext-switchev-iso15118/requirements.txt ./
RUN pip install --user -r requirements.txt

WORKDIR /opt/everest
COPY --from=builder /opt/everest ./

CMD [ "/opt/everest/bin/manager", "--conf", "/opt/everest/config/config.yaml" ]
