FROM ghcr.io/linuxcontainers/debian-slim:11

ENV PORT 80

RUN apt update && apt upgrade -y
RUN apt install -y \
    python3-pip \
    git \
    rsync \
    wget \
    cmake \
    doxygen \
    graphviz \
    build-essential \
    clang-tidy \
    cppcheck \
    maven \
    openjdk-11-jdk \
    npm \
    docker \
    docker-compose \
    libboost-all-dev \
    nodejs \
    libssl-dev \
    libsqlite3-dev \
    clang-format \
    curl \
    rfkill \
    inetutils-ping \
    nmap \
    openssh-client \
    python3-pytest \
    python3-pytest-dependency \
    python3-dateutil \
    python3-pytest-asyncio \
    python3-pyftpdlib \
    python3-openssl \
    libpcap-dev \
    sudo

ARG USERNAME=docker
ARG USER_UID=1000
RUN useradd --create-home --shell /bin/bash --uid ${USER_UID} --gid root ${USERNAME}
RUN usermod -aG sudo,root docker
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER ${USERNAME}

RUN python3 -m pip install --upgrade \
    pip==22.3 \
    setuptools==60.0 \
    wheel==0.37.1 \
    jsonschema==4.6.0

CMD [ "/bin/bash" ]
