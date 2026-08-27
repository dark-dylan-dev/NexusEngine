FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ARG RUNNER_VERSION=2.321.0

# --- build toolchains (same as before: gcc, clang, mingw for windows-approx) ---
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    gcc \
    g++ \
    clang \
    lldb \
    lld \
    mingw-w64 \
    ca-certificates \
    curl \
    jq \
    sudo \
    libicu-dev \
    && rm -rf /var/lib/apt/lists/*

# --- GitHub Actions self-hosted runner agent ---
RUN useradd -m runner && \
    mkdir -p /home/runner/actions-runner && \
    cd /home/runner/actions-runner && \
    curl -o actions-runner.tar.gz -L \
      "https://github.com/actions/runner/releases/download/v${RUNNER_VERSION}/actions-runner-linux-x64-${RUNNER_VERSION}.tar.gz" && \
    tar xzf actions-runner.tar.gz && \
    rm actions-runner.tar.gz && \
    ./bin/installdependencies.sh && \
    chown -R runner:runner /home/runner/actions-runner

USER runner
WORKDIR /home/runner/actions-runner

COPY --chown=runner:runner entrypoint.sh /home/runner/entrypoint.sh
ENTRYPOINT ["/home/runner/entrypoint.sh"]
