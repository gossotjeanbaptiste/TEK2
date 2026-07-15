#!/bin/bash

# BistroMatic - Execute example.bde file

echo "╔════════════════════════════════════════════════════════════╗"
echo "║         BISTROMATIC - EXECUTING example.bde                ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Check if bistroMatic binary exists
if [ ! -f ./bistroMatic ]; then
    echo "Building bistroMatic..."
    make fclean > /dev/null 2>&1
    make > /dev/null 2>&1
    if [ ! -f ./bistroMatic ]; then
        echo "Error: Failed to build bistroMatic"
        exit 1
    fi
fi

# Check if example.bde exists
if [ ! -f ./example.bde ]; then
    echo "Error: example.bde not found"
    exit 1
fi

echo "Running: ./bistroMatic example.bde"
echo "────────────────────────────────────────────────────────────"
echo ""

# Run the example file
./bistroMatic example.bde

echo ""
echo "────────────────────────────────────────────────────────────"
echo -e "\n✓ Execution completed"
