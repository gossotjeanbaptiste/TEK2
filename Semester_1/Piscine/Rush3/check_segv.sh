#!/bin/bash

# BistroMatic - Memory and Segmentation Fault Checker with Valgrind

echo "╔════════════════════════════════════════════════════════════╗"
echo "║    BISTROMATIC - MEMORY CHECK WITH VALGRIND                ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check if valgrind is installed
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}✗ Valgrind is not installed${NC}"
    echo "Install with: sudo apt-get install valgrind"
    exit 1
fi

echo -e "${YELLOW}[1/3] Cleaning previous builds...${NC}"
make fclean > /dev/null 2>&1

echo -e "${YELLOW}[2/3] Building bistroMatic...${NC}"
if ! make > /dev/null 2>&1; then
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Build successful${NC}"
echo ""

# Check if example.bde exists
if [ ! -f ./example.bde ]; then
    echo -e "${RED}✗ example.bde not found${NC}"
    exit 1
fi

echo -e "${YELLOW}[3/3] Running with Valgrind (this may take a moment)...${NC}"
echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║              VALGRIND OUTPUT                               ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Run with valgrind
# Options:
# --leak-check=full : Detailed leak info
# --show-leak-kinds=all : Show all leak kinds
# --track-origins=yes : Track origin of uninitialized values
# --verbose : Verbose output
# --log-file : Save output to file
VALGRIND_LOG="/tmp/valgrind_bistromatic.log"

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file="$VALGRIND_LOG" \
    ./bistroMatic example.bde 2>&1

echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║              VALGRIND SUMMARY                              ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Extract and display summary
if [ -f "$VALGRIND_LOG" ]; then
    echo -e "${BLUE}Memory Analysis Results:${NC}"
    echo "────────────────────────────────────────────────────────────"
    
    # Check for errors
    ERROR_COUNT=$(grep "ERROR SUMMARY" "$VALGRIND_LOG" | grep -oE '[0-9]+ error' | head -1)
    if [ -z "$ERROR_COUNT" ]; then
        ERROR_COUNT="0 errors"
    fi
    
    # Check for leaks
    LEAK_SUMMARY=$(grep "LEAK SUMMARY" -A 5 "$VALGRIND_LOG" | tail -4)
    
    if grep -q "ERROR SUMMARY: 0 errors" "$VALGRIND_LOG"; then
        echo -e "${GREEN}✓ No errors detected${NC}"
    else
        echo -e "${RED}✗ Errors found: $ERROR_COUNT${NC}"
        grep "ERROR SUMMARY" "$VALGRIND_LOG"
    fi
    
    echo ""
    if [ -n "$LEAK_SUMMARY" ]; then
        echo -e "Leak Summary:"
        echo "$LEAK_SUMMARY"
    fi
    
    echo ""
    echo "Full log saved to: $VALGRIND_LOG"
    echo ""
    
    # Show invalid access or use after free
    if grep -q "Invalid " "$VALGRIND_LOG" || grep -q "Use of uninitialised" "$VALGRIND_LOG"; then
        echo -e "${YELLOW}⚠ Issues found:${NC}"
        grep -E "Invalid |Use of uninitialised" "$VALGRIND_LOG" | head -10
        echo ""
    fi
    
    # Final verdict
    if grep -q "ERROR SUMMARY: 0 errors" "$VALGRIND_LOG" && grep -q "0 bytes" "$VALGRIND_LOG"; then
        echo -e "${GREEN}╔════════════════════════════════════════════════════════════╗${NC}"
        echo -e "${GREEN}║         ✓ MEMORY CHECK PASSED - NO LEAKS!                  ║${NC}"
        echo -e "${GREEN}╚════════════════════════════════════════════════════════════╝${NC}"
        exit 0
    else
        echo -e "${YELLOW}╔════════════════════════════════════════════════════════════╗${NC}"
        echo -e "${YELLOW}║         ⚠ MEMORY ISSUES DETECTED - CHECK LOG                ║${NC}"
        echo -e "${YELLOW}╚════════════════════════════════════════════════════════════╝${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ Failed to create Valgrind log${NC}"
    exit 1
fi
