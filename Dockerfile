# Build stage
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

# Copy project files
COPY CMakeLists.txt vcpkg.json ./
COPY src/ ./src/
COPY static/ ./static/
COPY tests/ ./tests/

# Build the project
RUN cmake -DCMAKE_BUILD_TYPE=Release . && \
    cmake --build . --config Release

# Runtime stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy compiled binary from builder
COPY --from=builder /build/pillio ./pillio

# Copy static files
COPY --from=builder /build/static/ ./static/

# Create data directory
RUN mkdir -p ./data/profiles

# Expose port
EXPOSE 8080

# Set environment variables
ENV API_PORT=8080
ENV STORAGE_PATH=/app/data/store.json
ENV FAMILY_STORAGE_PATH=/app/data/family.json

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/api/pills || exit 1

CMD ["./pillio"]
