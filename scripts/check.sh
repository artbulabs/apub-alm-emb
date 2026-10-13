#!/bin/bash
# =============================================================================
# check.sh - Local quality gate script
# Runs all checks that CI would run, before you push.
# Usage: ./scripts/check.sh
# =============================================================================
set -e

green='\033[0;32m'
cyan='\033[0;36m'
bold='\033[1m'
reset='\033[0m'

print_ok() {
    printf "      ${cyan}${bold}[$1/4] ${green}✓ OK${reset}\n"
}

repo_root="$(cd "$(dirname "${0}")/.." && pwd)"
cd "$repo_root"

printf "============================================"
printf "\n Alpha Alarm System - Local Quality Check"
printf "\n============================================"

# -----------------------------------------------------------------------------
printf "\n${cyan}${bold}[1/4]${reset} Checking code formatting (clang-format)...\n"
find src lib -name '*.cpp' -o -name '*.h' | \
    xargs clang-format --dry-run --Werror
print_ok 1

# -----------------------------------------------------------------------------
printf "\n${cyan}${bold}[2/4]${reset} Running static analysis (cppcheck)...\n"
cppcheck --enable=all --error-exitcode=1 \
    --suppress=missingIncludeSystem \
    --suppress=unmatchedSuppression \
    --suppress=unusedFunction:src/main.cpp \
    --suppress=unusedFunction:lib/alarm/AlarmStateMachine.cpp \
    --suppress=unusedFunction:lib/notify/StubNotifier.h \
    --suppress=unusedPrivateFunction:lib/notify/NtfyNotifier.h \
    --suppress=knownConditionTrueFalse:lib/notify/NtfyNotifier.cpp \
    -I include -I lib/alarm -I lib/notify -I lib/sensors \
    src lib
print_ok 2

# -----------------------------------------------------------------------------
printf "\n${cyan}${bold}[3/4]${reset} Running static analysis (clang-tidy)...\n"
clang-tidy lib/**/*.cpp \
    --header-filter="^${repo_root}/lib/.*" \
    -- -I include -I lib/alarm -I lib/notify -I lib/sensors \
    -D BOARD_NATIVE -std=c++17
print_ok 3

# -----------------------------------------------------------------------------
printf "\n${cyan}${bold}[4/4]${reset} Running unit tests with sanitizers (native_san)...\n"
pio test -e native_san
print_ok 4

# -----------------------------------------------------------------------------
printf "\n${green}${bold}"
printf "============================================\n"
printf " All checks passed!\n"
printf "============================================\n"
printf "${reset}\n"
printf " Tip: run ./scripts/coverage.sh for coverage report\n"
