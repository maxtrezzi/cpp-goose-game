#!/bin/sh
mkdir -p ./build && g++ -std=c++17 -Wall -Wextra -I ./src -I ./third_party/doctest -o ./build/goose_game_tests ./src/mt.cpp ./src/core.cpp ./src/view.cpp ./tests/test_main.cpp ./tests/core_test.cpp ./tests/view_test.cpp ./tests/mt_test.cpp && ./build/goose_game_tests "$@"
