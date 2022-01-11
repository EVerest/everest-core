ARG BASE_IMAGE=debian:11.0
FROM ${BASE_IMAGE}

ARG BASE_IMAGE
ARG USERNAME=docker
ARG USER_UID=1000

# create user and add to sudoers
RUN useradd -ms /bin/bash -u ${USER_UID} -U ${USERNAME}

RUN apt-get update \
    && apt-get install -y sudo \
    && echo ${USERNAME} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME}

RUN apt-get install -y \
    git \
    rsync \
    wget \
    cmake \
    doxygen \
    graphviz \
    build-essential \
    clang-tidy \
    cppcheck \
    libboost-all-dev \
    maven \
    openjdk-11-jdk \
    nodejs \
    npm

# create workdir and set access rights
WORKDIR /workspace/everest-cpp
RUN chown -R ${USERNAME}:${USERNAME} /workspace/everest-cpp

USER ${USERNAME}

# add github to known hosts
RUN mkdir ~/.ssh
RUN ssh-keyscan github.com > ~/.ssh/known_hosts

CMD [ "bash" ]