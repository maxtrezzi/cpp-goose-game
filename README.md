# Cpp Goose Game

The repository contains an implementation of the [goose game kata](https://github.com/xpeppers/goose-game-kata) with some additional commands.

## Prerequisites

In order to use the shell files included, you need a recent G++ compiler supporting C++ 17.
You should be able to build the project using other compilers without or with minor changes.

## How to play

Clone the repository on your local system, go to the project root folder and write:

```bash
./do-all.sh
```

`do-all.sh` builds the game (`do-build.sh`) and then starts it (`do-run.sh`).
After the first build you can use `./do-run.sh` directly.

## How to run the tests

The unit tests are in the `tests` folder and use [doctest](https://github.com/doctest/doctest)
(version 2.4.12, MIT license), included as a single header in `third_party/doctest`.
To build and run them, write:

```bash
./do-test.sh
```

You can pass doctest options to the script, for example `./do-test.sh --success` or `./do-test.sh -tc="Board"`.
