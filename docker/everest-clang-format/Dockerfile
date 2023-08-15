ARG BASE_IMAGE=debian:11
FROM ${BASE_IMAGE}

RUN apt update && apt upgrade -y

RUN apt install -y \
    python3 \
    python-is-python3 \
    python3-pip \
    git \
    libtinfo5

ADD https://github.com/EVerest/run-clang-format/releases/download/clang-format-16/clang-format \
    /usr/bin/clang-format
RUN chmod +x /usr/bin/clang-format

RUN git clone -b v1.0.0 https://@github.com/EVerest/run-clang-format.git
RUN cp run-clang-format/run-clang-format.py /usr/bin/run-clang-format

ENTRYPOINT ["/usr/bin/run-clang-format"]
