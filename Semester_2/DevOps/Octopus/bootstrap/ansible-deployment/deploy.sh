#!/bin/bash
# Script rapide pour exécuter le playbook

INVENTORY="../my-inventory"
PLAYBOOK="playbook.yml"

if [ "$1" == "--dry-run" ] || [ "$1" == "-c" ]; then
    echo "Running in check mode (dry-run)..."
    ansible-playbook -i "$INVENTORY" "$PLAYBOOK" --check -v
elif [ "$1" == "--syntax-check" ]; then
    echo "Checking syntax..."
    ansible-playbook -i "$INVENTORY" "$PLAYBOOK" --syntax-check
elif [ "$1" == "--help" ] || [ "$1" == "-h" ]; then
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --dry-run, -c       Run in check mode (no changes)"
    echo "  --syntax-check      Check playbook syntax only"
    echo "  --help, -h          Show this help message"
    echo "  (no args)           Run the playbook normally"
    echo ""
    echo "Examples:"
    echo "  $0                  # Execute playbook"
    echo "  $0 --dry-run        # Preview changes"
    echo "  $0 --syntax-check   # Validate syntax"
else
    echo "Running Ansible playbook..."
    ansible-playbook -i "$INVENTORY" "$PLAYBOOK" -v
fi
