#!/bin/sh
mkdir -p ./build && g++ -std=c++17 -Wall -Wextra -o ./build/goose_game ./src/mt.cpp ./src/core.cpp ./src/view.cpp ./src/main.cpp
