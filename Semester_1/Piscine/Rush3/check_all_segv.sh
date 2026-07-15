#!/bin/bash

# BistroMatic - Test all components with memory checking

echo "╔════════════════════════════════════════════════════════════╗"
echo "║      BISTROMATIC - COMPLETE MEMORY CHECK SUITE             ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Check if valgrind is installed
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}✗ Valgrind is not installed${NC}"
    echo "Install with: sudo apt-get install valgrind"
    exit 1
fi

# Create directory for logs
mkdir -p /tmp/valgrind_bistromatic
LOG_DIR="/tmp/valgrind_bistromatic"

echo -e "${YELLOW}[1/4] Building project...${NC}"
make fclean > /dev/null 2>&1
if ! make test > /dev/null 2>&1; then
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Build successful${NC}"
echo ""

# Function to run test with valgrind
run_test_with_valgrind() {
    local test_name=$1
    local test_binary=$2
    local log_file="$LOG_DIR/${test_name}.log"
    
    echo -e "${BLUE}Running: $test_name${NC}"
    
    valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --error-exitcode=42 \
        --log-file="$log_file" \
        ./"$test_binary" > /dev/null 2>&1
    
    local exit_code=$?
    
    if [ $exit_code -eq 0 ]; then
        echo -e "${GREEN}  ✓ No memory errors${NC}"
        return 0
    elif [ $exit_code -eq 42 ]; then
        echo -e "${RED}  ✗ Memory errors detected${NC}"
        return 1
    else
        echo -e "${YELLOW}  ⚠ Execution errors${NC}"
        return 2
    fi
}

# Run tests
echo -e "${YELLOW}[2/4] Running memory checks on test binaries...${NC}"
echo ""

FAILED=0

# Test operand
echo "Testing: test_operand"
run_test_with_valgrind "test_operand" "test_operand"
OPERAND_RESULT=$?
if [ $OPERAND_RESULT -ne 0 ]; then
    FAILED=$((FAILED + 1))
fi
echo ""

# Test parser
echo "Testing: test_parser"
run_test_with_valgrind "test_parser" "test_parser"
PARSER_RESULT=$?
if [ $PARSER_RESULT -ne 0 ]; then
    FAILED=$((FAILED + 1))
fi
echo ""

# Test VM
echo "Testing: test_vm"
run_test_with_valgrind "test_vm" "test_vm"
VM_RESULT=$?
if [ $VM_RESULT -ne 0 ]; then
    FAILED=$((FAILED + 1))
fi
echo ""

# Test main program
echo "Testing: bistroMatic (with example.bde)"
if [ -f ./example.bde ]; then
    valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --error-exitcode=42 \
        --log-file="$LOG_DIR/bistromatic.log" \
        ./bistroMatic example.bde > /dev/null 2>&1
    
    exit_code=$?
    
    if [ $exit_code -eq 0 ] || [ $exit_code -eq 1 ]; then
        echo -e "${GREEN}  ✓ No memory errors${NC}"
    elif [ $exit_code -eq 42 ]; then
        echo -e "${RED}  ✗ Memory errors detected${NC}"
        FAILED=$((FAILED + 1))
    fi
else
    echo -e "${YELLOW}  ⚠ example.bde not found, skipping${NC}"
fi
echo ""

echo -e "${YELLOW}[3/4] Analyzing results...${NC}"
echo ""

# Summarize
echo "╔════════════════════════════════════════════════════════════╗"
echo "║              MEMORY CHECK SUMMARY                          ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

echo -e "${BLUE}Test Results:${NC}"
echo "  test_operand: $([ $OPERAND_RESULT -eq 0 ] && echo -e "${GREEN}PASS${NC}" || echo -e "${RED}FAIL${NC}")"
echo "  test_parser:  $([ $PARSER_RESULT -eq 0 ] && echo -e "${GREEN}PASS${NC}" || echo -e "${RED}FAIL${NC}")"
echo "  test_vm:      $([ $VM_RESULT -eq 0 ] && echo -e "${GREEN}PASS${NC}" || echo -e "${RED}FAIL${NC}")"
echo ""

echo -e "${BLUE}Log Files:${NC}"
echo "  Location: $LOG_DIR/"
echo "  Files:"
ls -lh "$LOG_DIR"/ 2>/dev/null | awk '{if(NR>1) print "    - " $9 " (" $5 ")"}'
echo ""

echo -e "${YELLOW}[4/4] Final verdict...${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║    ✓ ALL MEMORY CHECKS PASSED - NO LEAKS DETECTED!         ║${NC}"
    echo -e "${GREEN}╚════════════════════════════════════════════════════════════╝${NC}"
    exit 0
else
    echo -e "${RED}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${RED}║    ✗ $FAILED TEST(S) FAILED - MEMORY ISSUES FOUND        ║${NC}"
    echo -e "${RED}╚════════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo "To view detailed logs, check: $LOG_DIR/"
    exit 1
fi
