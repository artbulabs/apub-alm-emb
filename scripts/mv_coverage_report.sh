#!/bin/bash

# Moves the coverage_report directory to the destination specified by dst_dir.
# The directory is renamed with its creation date and time.

# Set the destination directory for the coverage report.
dst_dir=""

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_dir="$(dirname "$script_dir")"

src="$project_dir/coverage_report"

if [ -z "$dst_dir" ]; then
    echo "Destination directory is not configured"
    exit 1
fi

mkdir -p "$dst_dir"

creation_date=$(stat -c %w "$src")

if [ "$creation_date" = "-" ]; then
    echo "Cannot read the directory creation time: $src"
    exit 1
fi

dst="$dst_dir/coverage_report_$(date -d "$creation_date" +%Y-%m-%d_%H-%M-%S)"
mv "$src" "$dst"
