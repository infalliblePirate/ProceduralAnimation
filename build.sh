#!/bin/bash

# Function to check if a package is installed
check_and_install() {
    local package=$1
    local install_command=$2

    if ! dpkg-query -W -f='${Status}' $package 2>/dev/null | grep "install ok installed" >/dev/null 2>&1 && 
       ! rpm -q $package >/dev/null 2>&1 && 
       ! pacman -Qs $package >/dev/null 2>&1; then
        echo "$package is not installed. Installing..."
        eval "$install_command"
    else
        echo "$package is already installed."
    fi
}

# Detect the package manager
if command -v dnf >/dev/null; then
    PKG_MANAGER="dnf"
    INSTALL_CMD="sudo dnf install -y"
elif command -v apt >/dev/null; then
    PKG_MANAGER="apt"
    INSTALL_CMD="sudo apt install -y"
elif command -v pacman >/dev/null; then
    PKG_MANAGER="pacman"
    INSTALL_CMD="sudo pacman -S --noconfirm"
else
    echo "Unsupported package manager. Please install manually."
    exit 1
fi

# Check and install g++
if command -v g++ >/dev/null 2>&1; then
    echo "g++ is already installed."
else
    echo "g++ is not installed. Installing..."
    if [ "$PKG_MANAGER" == "dnf" ]; then
        sudo dnf groupinstall "Development Tools" -y
        sudo dnf install gcc-c++ -y
    elif [ "$PKG_MANAGER" == "apt" ]; then
        sudo apt update
        sudo apt install build-essential g++ -y
    elif [ "$PKG_MANAGER" == "pacman" ]; then
        sudo pacman -S base-devel gcc --noconfirm
    else
        echo "Unsupported package manager. Please install g++ manually."
        exit 1
    fi
fi

# List of other packages to check/install
packages=(
    "wayland-devel"
    "wayland-protocols-devel"
    "libxkbcommon-devel"
    "libX11-devel"
    "libXrandr-devel"
    "libXinerama-devel"
    "libXcursor-devel"
    "libXi-devel"
    "mesa-libGL-devel"
    "mesa-libGLU-devel"
)

# Loop through the packages and install if missing
for package in "${packages[@]}"; do
    check_and_install "$package" "$INSTALL_CMD $package"
done

# The directory where the engine is located
ENGINE_DIR="Deimos"

# Clone the engine repository if it doesn't exist
if [ -d "$ENGINE_DIR" ]; then
    echo "Engine directory already exists. Skipping fetch."
else 
    git clone --branch v1.0.0 https://github.com/infalliblePirate/Engine.git "$ENGINE_DIR"
fi

# Create build directory if it doesn't exist
BUILD_DIR="build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug # Alternatively DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build .
