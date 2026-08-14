#!/bin/bash
# =============================================================================
# coverage.sh - Create coverage report
# Runs tests with code coverage and generates an HTML coverage report.
# The report is generated in the coverage_report directory.
# Usage: ./scripts/coverage.sh
# =============================================================================
set -e

build_dir=".pio/build/native_cov"
report_dir="coverage_report"

echo "=== Cleaning previous build ==="
pio run -e native_cov -t clean

echo "=== Running tests with coverage ==="
pio test -e native_cov

echo "=== Collecting coverage data ==="
lcov --capture \
     --directory "$build_dir" \
     --output-file coverage.info \
     --branch-coverage \
     --ignore-errors mismatch,inconsistent,deprecated

echo "=== Filtering: keeping only our source files ==="
lcov --remove coverage.info \
     '*/googletest/*' \
     '*/test/*' \
     '/usr/*' \
     --output-file coverage_filtered.info \
     --branch-coverage \
     --ignore-errors mismatch,inconsistent,deprecated,unused

echo "=== Coverage summary ==="
lcov --list coverage_filtered.info \
     --branch-coverage \
     --ignore-errors mismatch,inconsistent,deprecated

echo "=== Generating HTML report ==="
genhtml coverage_filtered.info \
        --output-directory "$report_dir" \
        --branch-coverage \
        --title "Alpha Alarm System Coverage"

echo "=== Done! Open $report_dir/index.html in browser ==="
