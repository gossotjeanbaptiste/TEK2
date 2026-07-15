#!/bin/bash
# Pre-deployment checklist for Ansible infrastructure

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║        Step 2 - Pre-Deployment Checklist                      ║"
echo "║        G-DOP-400 Octopus Bootstrap                            ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

CHECKS_PASSED=0
CHECKS_FAILED=0

# Function to check a condition
check() {
    local name=$1
    local command=$2
    local expected=$3
    
    echo -n "Checking: $name ... "
    
    if eval "$command" > /dev/null 2>&1; then
        echo "✅ PASS"
        ((CHECKS_PASSED++))
        return 0
    else
        echo "❌ FAIL"
        echo "  → Run: $command"
        ((CHECKS_FAILED++))
        return 1
    fi
}

echo "════════════════════════════════════════════════════════════════"
echo "1. System Requirements"
echo "════════════════════════════════════════════════════════════════"
check "Ansible installed" "ansible --version" ""
check "Python3 installed" "python3 --version" ""
check "SSH installed" "ssh -V" ""
echo ""

echo "════════════════════════════════════════════════════════════════"
echo "2. Project Structure"
echo "════════════════════════════════════════════════════════════════"
check "Inventory file exists" "test -f ../my-inventory" ""
check "Playbook exists" "test -f playbook.yml" ""
check "Base role exists" "test -d roles/base/tasks" ""
check "Redis role exists" "test -d roles/redis/tasks" ""
check "Node.js role exists" "test -d roles/nodejs/tasks" ""
check "Deploy role exists" "test -d roles/deploy/tasks" ""
check "SSH key exists" "test -f ~/.ssh/id_ed25519" ""
echo ""

echo "════════════════════════════════════════════════════════════════"
echo "3. SSH Configuration"
echo "════════════════════════════════════════════════════════════════"
check "SSH key readable" "test -r ~/.ssh/id_ed25519" ""
check "SSH public key exists" "test -f ~/.ssh/id_ed25519.pub" ""
echo ""

echo "════════════════════════════════════════════════════════════════"
echo "4. Inventory Configuration"
echo "════════════════════════════════════════════════════════════════"
check "Web hosts in inventory" "grep -q '\[web\]' ../my-inventory" ""
check "Redis hosts in inventory" "grep -q '\[redis\]' ../my-inventory" ""
check "Ansible vars configured" "grep -q 'ansible_user' ../my-inventory" ""
echo ""

echo "════════════════════════════════════════════════════════════════"
echo "5. Host Connectivity (Manual Check Recommended)"
echo "════════════════════════════════════════════════════════════════"
echo "Run the following commands manually to test connectivity:"
echo ""
echo "  ansible -i ../my-inventory all -m ping"
echo ""
echo "Expected: All 3 hosts should respond with SUCCESS"
echo ""

echo "════════════════════════════════════════════════════════════════"
echo "6. Playbook Syntax"
echo "════════════════════════════════════════════════════════════════"
check "Playbook syntax valid" "ansible-playbook playbook.yml --syntax-check" ""
echo ""

echo "════════════════════════════════════════════════════════════════"
echo "Results Summary"
echo "════════════════════════════════════════════════════════════════"
TOTAL=$((CHECKS_PASSED + CHECKS_FAILED))
echo "Total Checks: $TOTAL"
echo "Passed: ✅ $CHECKS_PASSED"
echo "Failed: ❌ $CHECKS_FAILED"
echo ""

if [ $CHECKS_FAILED -eq 0 ]; then
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║  ✅ All checks passed! Ready to deploy.                       ║"
    echo "║                                                                ║"
    echo "║  Next step: Run                                                ║"
    echo "║    bash deploy.sh                                              ║"
    echo "║  or                                                            ║"
    echo "║    ansible-playbook -i ../my-inventory playbook.yml -v        ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
    exit 0
else
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║  ❌ Some checks failed. Please fix the issues above.          ║"
    echo "║                                                                ║"
    echo "║  Fix the failing checks and run this script again.             ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
    exit 1
fi
