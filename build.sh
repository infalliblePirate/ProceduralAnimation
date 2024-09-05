#!/bin/bash

# The directory where the engine is located
ENGINE_DIR="Deimos"

# CLone the engine repository if it doesn't exist
if [ -d "$ENGINE_DIR" ]; then
    echo "Engine directory already exists. Skipping fetch."
else 
    git clone https://github.com/infalliblePirate/Engine.git "$ENGINE_DIR"
fi

# Create build directory if it doesn't exist
BUILD_DIR="build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure Cmake
cmake .. -DCMAKE_BUILD_TYPE=Debug # Alternatively DCMAKE_BUILD_TYPE=Release

cmake --build .