# syntax=docker/dockerfile:1.7
# SPDX-License-Identifier: MIT
# Two-stage build: the first stage compiles with a full toolchain, the
# second stage ships only the binaries + runtime libs to keep the image lean.

# ---------- Stage 1: builder ----------
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        g++ \
        cmake \
        make \
        libssl-dev \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY CMakeLists.txt ./
COPY lib/    ./lib/
COPY data/   ./data/
COPY src/    ./src/

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j"$(nproc)" \
    && cmake --install build --prefix /out

# ---------- Stage 2: runtime ----------
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        libssl3 \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --system --create-home --shell /bin/bash pp

WORKDIR /app
COPY --from=builder /out/bin/      ./bin/
COPY --from=builder /out/share/    ./share/

RUN mkdir -p /app/log/info.log /app/log/dataLog \
    && chown -R pp:pp /app

USER pp

# Default entrypoint is the bash shell so the operator can pick the binary.
CMD ["/bin/bash"]
