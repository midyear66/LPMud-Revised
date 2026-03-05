FROM debian:bookworm-slim AS builder

# Install build dependencies for ldmud
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    autoconf \
    automake \
    bison \
    pkg-config \
    git \
    ca-certificates \
    libssl-dev \
    libpcre3-dev \
    libjson-c-dev \
    && rm -rf /var/lib/apt/lists/*

# Clone and build ldmud
WORKDIR /build
RUN git clone --depth 1 https://github.com/ldmud/ldmud.git

WORKDIR /build/ldmud/src
RUN ./autogen.sh \
    && ./configure \
        --prefix=/usr/local \
        --enable-compat \
    && make \
    && make install

# --- Runtime stage ---
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    libssl3 \
    libpcre3 \
    libjson-c5 \
    && rm -rf /var/lib/apt/lists/*

# Copy the built driver
COPY --from=builder /usr/local/bin/ldmud /usr/local/bin/ldmud

# Create mud user (don't run as root)
RUN useradd -r -m -d /mud -s /bin/bash mud

# Set up mudlib directory
WORKDIR /mud
RUN mkdir -p /mud/lib /mud/log /mud/tmp /mud/lib/players

# Copy mudlib and config (provided via context or volume)
COPY --chown=mud:mud mudlib/ /mud/lib/
COPY --chown=mud:mud config/ldmud.conf /mud/ldmud.conf

RUN chown -R mud:mud /mud

USER mud

EXPOSE 4000

CMD ["ldmud", "--args", "/mud/ldmud.conf"]
