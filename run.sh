#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

echo "==> Configuring..."
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR"

echo "==> Building..."
cmake --build "$BUILD_DIR" -- -j$(nproc)

echo "==> Running..."
"$BUILD_DIR/opengl_project"
