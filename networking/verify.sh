#!/bin/bash

# Quick Verification Script for S.H.A.M. Protocol
# Run this to quickly check if your implementation meets basic requirements

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}=== S.H.A.M. Protocol Quick Verification ===${NC}"
echo ""

# Check 1: Executables exist
echo -e "${YELLOW}1. Checking executables...${NC}"
if [ -f "./server" ] && [ -f "./client" ]; then
    echo -e "${GREEN}✓ Executables found${NC}"
else
    echo -e "${RED}✗ Missing server or client executable${NC}"
    echo "Run: make clean && make"
    exit 1
fi

# Check 2: Basic CLI
echo -e "${YELLOW}2. Checking command-line interface...${NC}"
if ./server 2>&1 | grep -q "Usage\|usage\|port"; then
    echo -e "${GREEN}✓ Server CLI works${NC}"
else
    echo -e "${RED}✗ Server CLI not working${NC}"
fi

if ./client 2>&1 | grep -q "Usage\|usage\|server_ip"; then
    echo -e "${GREEN}✓ Client CLI works${NC}"
else
    echo -e "${RED}✗ Client CLI not working${NC}"
fi

# Check 3: Header files
echo -e "${YELLOW}3. Checking header files...${NC}"
if [ -f "./include/sham.h" ] || [ -f "./include/network_utils.h" ]; then
    echo -e "${GREEN}✓ Header files exist${NC}"
else
    echo -e "${RED}✗ Missing header files${NC}"
fi

# Check 4: Source files
echo -e "${YELLOW}4. Checking source files...${NC}"
required_files=("client.c" "server.c" "sham_protocol.c")
missing_files=()
for file in "${required_files[@]}"; do
    if [ ! -f "./src/$file" ]; then
        missing_files+=("$file")
    fi
done

if [ ${#missing_files[@]} -eq 0 ]; then
    echo -e "${GREEN}✓ All required source files present${NC}"
else
    echo -e "${RED}✗ Missing source files: ${missing_files[*]}${NC}"
fi

# Check 5: Makefile
echo -e "${YELLOW}5. Checking Makefile...${NC}"
if [ -f "./Makefile" ]; then
    echo -e "${GREEN}✓ Makefile exists${NC}"
else
    echo -e "${RED}✗ Makefile missing${NC}"
fi

# Check 6: Compilation flags
echo -e "${YELLOW}6. Checking compilation...${NC}"
if grep -q "lcrypto" Makefile 2>/dev/null; then
    echo -e "${GREEN}✓ OpenSSL linking found${NC}"
else
    echo -e "${RED}✗ OpenSSL linking not found in Makefile${NC}"
fi

echo ""
echo -e "${GREEN}=== Quick Check Complete ===${NC}"
echo "For comprehensive testing, run: ./test_networking.sh"
echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo "1. If all checks passed: Run the full test suite"
echo "2. If executables missing: Run 'make clean && make'"
echo "3. If compilation issues: Check Makefile and source files"
echo "4. For detailed testing: Run './test_networking.sh'"