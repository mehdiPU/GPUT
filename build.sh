#!/bin/bash

set -e

mkdir -p build && cd build

cmake .. \
  -DCMAKE_BUILD_TYPE=Debug\
  -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
cmake --build .

cp compile_commands.json ..
