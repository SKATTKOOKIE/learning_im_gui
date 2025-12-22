#!/bin/bash
# setup.sh - Initialize and configure git submodules for the project

set -e  # Exit on error

echo "================================================"
echo "Setting up project dependencies..."
echo "================================================"

# Check if git is available
if ! command -v git &> /dev/null; then
    echo "Error: git is not installed"
    exit 1
fi

# Initialize and update all submodules
echo ""
echo "Initializing git submodules..."
git submodule update --init --recursive

# Configure Boost
echo ""
echo "Configuring Boost..."
cd external/boost
git checkout boost-1.83.0
echo "Checked out Boost 1.83.0"
git submodule update --init --recursive
echo "Initialized Boost submodules"
cd ../..

# Configure GLFW
echo ""
echo "Configuring GLFW..."
cd external/glfw
git checkout 3.4
echo "Checked out GLFW 3.4"
cd ../..

# Configure ImGui (docking branch)
echo ""
echo "Configuring ImGui..."
cd external/imgui
git checkout docking
echo "Checked out ImGui docking branch"
cd ../..

echo ""
echo "================================================"
echo "Setup complete!"
echo "================================================"
echo ""
echo "You can now build the project:"
echo "  mkdir -p build"
echo "  cd build"
echo "  cmake .."
echo "  make -j\$(nproc)"
echo ""