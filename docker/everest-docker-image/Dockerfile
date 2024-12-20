# syntax=docker/dockerfile:1
FROM debian:12-slim AS builder

ARG REPO
ARG BRANCH
ARG EVEREST_CONFIG
ARG OCPP_CONFIG
ARG ADDITIONAL_CMAKE_PARAMETERS
ARG EV_CLI_VERSION
ARG INSTALL_EV_CLI

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
    libpcap-dev \
    libcap-dev \
    python3-venv \
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
ENV EVEREST_VENV=/workspace/everest/venv
RUN python3 -m venv ${EVEREST_VENV}
ENV PATH="${EVEREST_VENV}/bin:${PATH}"
RUN git clone https://github.com/EVerest/everest-cmake.git
RUN git clone https://github.com/EVerest/everest-utils.git -b ${EV_CLI_VERSION}
WORKDIR /workspace/everest/everest-utils/ev-dev-tools
RUN if [ "${INSTALL_EV_CLI}" = "install_ev_cli" ] ; then python3 -m pip install . ; fi
WORKDIR /workspace/everest
RUN git clone https://github.com/EVerest/everest-dev-environment.git
WORKDIR /workspace/everest/everest-dev-environment/dependency_manager
RUN python3 -m pip install .
WORKDIR /workspace/everest
RUN git clone https://github.com/EVerest/ext-switchev-iso15118.git
WORKDIR /workspace/everest/ext-switchev-iso15118/
RUN pip install -r requirements.txt

WORKDIR /workspace/everest

RUN rm -rf "/workspace/everest/$(basename "${REPO}" .git)"
RUN --mount=type=ssh git clone ${REPO}

RUN rm -rf "/workspace/everest/$(basename "${REPO}" .git)/build" && \
    cd "/workspace/everest/$(basename "${REPO}" .git)" && \
    git checkout "${BRANCH}" && \
    mkdir "/workspace/everest/$(basename "${REPO}" .git)/build" && \
    cd "/workspace/everest/$(basename "${REPO}" .git)/build" && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/opt/everest ${ADDITIONAL_CMAKE_PARAMETERS} && \
    make -j"$(nproc)" install

# Try to copy the OCPP 2.0.1 config directory to have the init_device_model_db.py script available for (re-)initialization of the device model
RUN cp -R "$(grep -m 1 "ocpp_SOURCE_DIR:STATIC=" "/workspace/everest/$(basename "${REPO}" .git)/build/CMakeCache.txt" | sed "s/ocpp_SOURCE_DIR:STATIC=//")/config/v201" /opt/everest/ocpp201config || echo "Could not copy OCPP 2.0.1 config directory"
RUN mkdir -p /opt/everest/config/user-config
COPY "${EVEREST_CONFIG}" /opt/everest/config/
COPY "${OCPP_CONFIG}" /opt/everest/config/
RUN if [ "${EVEREST_CONFIG}" != "config.yaml" ]; then mv /opt/everest/config/"${EVEREST_CONFIG}" /opt/everest/config/config.yaml ; fi
RUN if [ "${OCPP_CONFIG}" != "ocpp-config.json" ]; then mv /opt/everest/config/"${OCPP_CONFIG}" /opt/everest/config/ocpp-config.json ; fi

COPY logging.ini /opt/everest/config

# syntax=docker/dockerfile:1
FROM debian:12-slim

RUN apt-get update \
    && apt-get install --no-install-recommends -y \
    openjdk-17-jre \
    nodejs \
    npm \
    python3-pip \
    sqlite3 \
    libboost-program-options1.74.0 \
    libboost-log1.74.0 \
    libboost-chrono1.74.0 \
    libboost-system1.74.0 \
    libssl3 \
    libcurl4 \
    libcap2 \
    less \
    libevent-dev \
    python3-venv \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

ENV EVEREST_VENV=/workspace/everest/venv
RUN python3 -m venv ${EVEREST_VENV}
ENV PATH="${EVEREST_VENV}/bin:${PATH}"
COPY --from=builder /workspace/everest/venv /workspace/everest/venv

WORKDIR /opt/everest
COPY --from=builder /opt/everest ./
COPY ./scripts/initialize.sh /opt/everest

CMD [ "/opt/everest/bin/manager", "--conf", "/opt/everest/config/config.yaml" ]
