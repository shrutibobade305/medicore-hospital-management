# Multi-stage Dockerfile for MediCore C++ Crow Backend on Render

# ----------------------------------------------------
# Stage 1: Build Environment
# ----------------------------------------------------
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install essential C++ build tools, CMake, and compiler
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    g++ \
    cmake \
    make \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy build configuration and backend source code
COPY CMakeLists.txt ./
COPY include/ ./include/
COPY src/ ./src/

# Compile the medicore_server executable using CMake in Release mode
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) medicore_server

# ----------------------------------------------------
# Stage 2: Minimal Production Runtime
# ----------------------------------------------------
FROM ubuntu:22.04 AS runner

ENV DEBIAN_FRONTEND=noninteractive

# Install runtime support libraries
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    libstdc++6 \
    tzdata \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Create persistent data directory for SQLite database
RUN mkdir -p data

# Copy compiled C++ binary from builder stage
COPY --from=builder /app/build/medicore_server /app/medicore_server

# Grant execution permissions
RUN chmod +x /app/medicore_server

# Default environment configuration for Render deployment
ENV PORT=10000
ENV DATABASE_PATH=/app/data/hospital.db

EXPOSE 10000

# Container healthcheck
HEALTHCHECK --interval=30s --timeout=5s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:${PORT}/api/health || exit 1

# Run the C++ server
CMD ["./medicore_server"]
