#!/bin/bash
cd "$(dirname "$0")"
cd ../source
set -e
echo --- BUILD START ---;
make $1
echo --- BUILD SUCCESS ---;