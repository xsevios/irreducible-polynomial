#!/bin/bash
cd "$(dirname "$0")"
set -e
./build.sh $1
echo ;
echo --- RUN ---;
./run.sh
echo --- RUN SUCCESS ---;