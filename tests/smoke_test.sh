#!/usr/bin/env bash
set -euo pipefail

output=$(printf '4\n3\n0\n8\n1\n4\n2\n2\n1\n3\n1\n4\nn\n' | ./scheduler)

grep -q 'Gantt Chart:' <<< "$output"
grep -q 'Average Turnaround Time:' <<< "$output"
grep -q 'Average Waiting Time:' <<< "$output"
grep -q 'Average Response Time:' <<< "$output"
grep -q 'CPU Utilization:' <<< "$output"

echo "Smoke test passed."
