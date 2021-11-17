#!/bin/bash

cd "$(dirname "$0")"
cd ../build
cat out | awk '{getline line < "judge"; split(line,s); if (s[1]!=$1) {print line,",\t", $0}}'
