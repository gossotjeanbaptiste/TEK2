#!/bin/bash
# Script de validation et de test pour la deployment Ansible

set -e

INVENTORY="../my-inventory"
PLAYBOOK="playbook.yml"
ANSIBLE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=================================================="
echo "   Ansible Deployment Validation Script"
echo "=================================================="
echo ""

# Couleurs pour l'output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Vérifier si Ansible est installé
if ! command -v ansible &> /dev/null; then
    echo -e "${RED}[ERROR]${NC} Ansible n'est pas installé"
    exit 1
fi

# Vérifier si ansible-playbook est disponible
if ! command -v ansible-playbook &> /dev/null; then
    echo -e "${RED}[ERROR]${NC} ansible-playbook n'est pas disponible"
    exit 1
fi

echo -e "${GREEN}[OK]${NC} Ansible is installed"
ansible --version
echo ""

# Test 1: Vérifier la syntaxe du playbook
echo "=================================================="
echo "Test 1: Checking playbook syntax..."
echo "=================================================="
if ansible-playbook -i "$INVENTORY" "$PLAYBOOK" --syntax-check; then
    echo -e "${GREEN}[OK]${NC} Playbook syntax is valid"
else
    echo -e "${RED}[ERROR]${NC} Playbook syntax error"
    exit 1
fi
echo ""

# Test 2: Vérifier la connexion aux hôtes
echo "=================================================="
echo "Test 2: Checking host connectivity..."
echo "=================================================="
if ansible -i "$INVENTORY" all -m ping; then
    echo -e "${GREEN}[OK]${NC} All hosts are reachable"
else
    echo -e "${RED}[WARNING]${NC} Some hosts may not be reachable"
fi
echo ""

# Test 3: Exécuter le playbook en mode dry-run
echo "=================================================="
echo "Test 3: Running playbook in check mode (dry-run)..."
echo "=================================================="
if ansible-playbook -i "$INVENTORY" "$PLAYBOOK" --check -v; then
    echo -e "${GREEN}[OK]${NC} Playbook dry-run successful"
else
    echo -e "${YELLOW}[WARNING]${NC} Some tasks may have failed in check mode"
    echo "This might be expected for certain operations (e.g., waiting for ports)"
fi
echo ""

# Test 4: Afficher les informations sur les hôtes
echo "=================================================="
echo "Test 4: Host information..."
echo "=================================================="
echo "Web servers:"
ansible -i "$INVENTORY" web -m debug -a "msg='{{ ansible_host }}'"
echo ""
echo "Cache servers:"
ansible -i "$INVENTORY" redis -m debug -a "msg='{{ ansible_host }}'"
echo ""

# Test 5: Lister les rôles disponibles
echo "=================================================="
echo "Test 5: Available roles..."
echo "=================================================="
ls -la roles/
echo ""

echo "=================================================="
echo -e "${GREEN}[SUCCESS]${NC} All validation tests completed!"
echo "=================================================="
echo ""
echo "To deploy the infrastructure, run:"
echo "  ansible-playbook -i $INVENTORY $PLAYBOOK -v"
echo ""
