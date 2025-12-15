#!/bin/bash
# Build WASM files for tree-sitter-fsharp
# This script can be run with WSL on Windows or natively on Linux/macOS

set -e

# Check if tree-sitter is available
if ! command -v tree-sitter &> /dev/null; then
    echo "Error: tree-sitter CLI not found"
    echo "Please install it with: cargo install tree-sitter-cli"
    echo "Or download from: https://github.com/tree-sitter/tree-sitter/releases"
    exit 1
fi

# Setup Emscripten if available
if [ -f "/tmp/emsdk/emsdk_env.sh" ]; then
    export EMSDK_QUIET=1
    source /tmp/emsdk/emsdk_env.sh
fi

# Create wasm directory if it doesn't exist
mkdir -p wasm

echo "Building WASM for fsharp grammar..."
cd fsharp
tree-sitter build --wasm
mv -f tree-sitter-fsharp.wasm ../wasm/ 2>/dev/null || true
cd ..

echo "Building WASM for fsharp_signature grammar..."
cd fsharp_signature
tree-sitter build --wasm
mv -f tree-sitter-fsharp_signature.wasm ../wasm/ 2>/dev/null || true
cd ..

echo ""
echo "WASM files built successfully in wasm/ directory:"
if [ -f wasm/tree-sitter-fsharp.wasm ]; then
    ls -lh wasm/tree-sitter-fsharp.wasm | awk '{print "  - wasm/tree-sitter-fsharp.wasm (" $5 ")"}'
fi
if [ -f wasm/tree-sitter-fsharp_signature.wasm ]; then
    ls -lh wasm/tree-sitter-fsharp_signature.wasm | awk '{print "  - wasm/tree-sitter-fsharp_signature.wasm (" $5 ")"}'
fi
echo ""
