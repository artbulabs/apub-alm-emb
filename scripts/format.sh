#!/bin/bash
# =============================================================================
# format.sh - Auto-format all source files using clang-format
#
# Replaces manual commands:
#   Format: find src lib -name '*.cpp' -o -name '*.h' | xargs clang-format -i
#   Check:  find src lib -name '*.cpp' -o -name '*.h' | xargs clang-format --dry-run --Werror
#
# Usage:
#   ./scripts/format.sh          - format all files in place
#   ./scripts/format.sh --check  - check only, no changes (used in CI)
# =============================================================================

repo_root="$(cd "$(dirname "${0}")/.." && pwd)"
cd "$repo_root"

files=$(find src lib -name '*.cpp' -o -name '*.h')

if [ "${1}" = "--check" ]; then
    echo "Checking code formatting..."
    if echo "$files" | xargs clang-format --dry-run --Werror; then
        echo "OK - all files are properly formatted"
    else
        echo "FAILED - some files need formatting, run: ./scripts/format.sh"
        exit 1
    fi
else
    echo "Formatting all source files..."
    echo "$files" | xargs clang-format -i
    echo "Done - all files formatted"
fi
