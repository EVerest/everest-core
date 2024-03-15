ARG BASE_IMAGE=debian:12
FROM ${BASE_IMAGE}

RUN apt update && apt upgrade -y
RUN apt update && \
    apt install -y \
        clang-format-15=1:15.0.6*
RUN ln -s /usr/bin/clang-format-15 /usr/bin/clang-format

RUN apt install -y \
    python-is-python3

COPY run-clang-format.py /usr/bin/run-clang-format

ENTRYPOINT ["/usr/bin/run-clang-format"]
