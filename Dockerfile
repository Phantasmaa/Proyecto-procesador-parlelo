FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    build-essential \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY src/ ./src/
COPY lib/ ./lib/
COPY data/ ./data/
COPY log/ ./log/

RUN mkdir -p bin log log/dataLog && \
    g++ -o bin/sender src/sender.cpp \
    -I lib -I data \
    -lssl -lcrypto -lpthread && \
    g++ -o bin/receiver src/receiver.cpp \
    -I lib -I data \
    -lssl -lcrypto -lpthread && \
    chmod +x src/com.sh && chmod 777 log

# Compile once at build time, run the binary at runtime
RUN chmod +x src/com.sh

CMD ["/bin/bash"]
