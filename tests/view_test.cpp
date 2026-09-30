#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "doctest.h"
#include "view.hpp"

using namespace goose_game::view;

namespace {

  // Replaces std::cin and std::cout while it is alive
  class ConsoleRedirect {
    public:
      explicit ConsoleRedirect(const std::string& input) : in {input},
          oldIn {std::cin.rdbuf(in.rdbuf())}, oldOut {std::cout.rdbuf(out.rdbuf())} {
      }

      ~ConsoleRedirect() {
          std::cin.rdbuf(oldIn);
          std::cout.rdbuf(oldOut);
          std::cin.clear();
      }

      std::string output() const {
          return out.str();
      }
    private:
      std::istringstream in;
      std::ostringstream out;
      std::streambuf* oldIn;
      std::streambuf* oldOut;
  };

  // Runs the application with the given input and returns what it prints.
  // The checks must run after the redirect ends: doctest writes its report on std::cout.
  std::string runApp(const std::string& input) {
      ConsoleRedirect console {input};
      AppView().show();
      return console.output();
  }

  bool contains(const std::string& text, const std::string& part) {
      return text.find(part) != std::string::npos;
  }
}

TEST_CASE("MoveArgs parsing") {
    SUBCASE("player and dice") {
        MoveArgs args = MoveArgs::parseMoveArgs(" Pippo 4, 2");
        CHECK(args.isComplete());
        CHECK(args.getPlayerName() == "Pippo");
        CHECK(args.getFirstDice() == 4);
        CHECK(args.getSecondDice() == 2);
    }

    SUBCASE("player only") {
        MoveArgs args = MoveArgs::parseMoveArgs(" Pippo");
        CHECK_FALSE(args.isComplete());
        CHECK(args.getPlayerName() == "Pippo");
    }

    SUBCASE("invalid arguments") {
        CHECK_THROWS_WITH_AS(MoveArgs::parseMoveArgs(""),
                             "Command Move: Player's name is required\n", std::invalid_argument);
        CHECK_THROWS_WITH_AS(MoveArgs::parseMoveArgs(" Pippo 7, 1"),
                             "Invalid dice argument: 7\n", std::invalid_argument);
        CHECK_THROWS_WITH_AS(MoveArgs::parseMoveArgs(" Pippo x, 1"),
                             "Invalid dice argument: x\n", std::invalid_argument);
        CHECK_THROWS_WITH_AS(MoveArgs::parseMoveArgs(" Pippo 99999999999, 1"),
                             "Invalid dice argument: 99999999999\n", std::invalid_argument);
        CHECK_THROWS_WITH_AS(MoveArgs::parseMoveArgs(" Pippo 1"),
                             "Command Move: Invalid arguments\n", std::invalid_argument);
        CHECK_THROWS_AS(MoveArgs::parseMoveArgs(" Pippo 1, 2, 3"), std::invalid_argument);
    }
}

TEST_CASE("AppView") {
    SUBCASE("adds players and exits") {
        std::string output = runApp("add player Pippo\nadd player Pippo\nfoo\nexit\n");
        CHECK(contains(output, "Player Pippo successfully added"));
        CHECK(contains(output, "Pippo: already existing player"));
        CHECK(contains(output, "Unknown command"));
        CHECK(contains(output, "Bye Bye"));
    }

    SUBCASE("stops at the end of the input") {
        std::string output = runApp("add player Pippo\nplay\n");
        CHECK_FALSE(contains(output, "Bye Bye"));
    }

    SUBCASE("a command must be followed by a space") {
        std::string output = runApp("add playerPippo\nadd player\nexit\n");
        CHECK_FALSE(contains(output, "successfully added"));
        CHECK(contains(output, "Unknown command"));
        CHECK(contains(output, "Player's name is required"));
    }

    SUBCASE("does not start a game without players") {
        std::string output = runApp("play\nexit\n");
        CHECK(contains(output, "No players for the game"));
    }

    SUBCASE("the move command must be followed by a space") {
        std::string output = runApp("add player Pippo\nplay\nmovePippo 1, 2\nmove\nexit\nexit\n");
        CHECK_FALSE(contains(output, "Pippo rolls"));
        CHECK(contains(output, "Unknown command"));
        CHECK(contains(output, "Command Move: Player's name is required"));
    }

    SUBCASE("plays a whole game") {
        std::string output = runApp("add player Pippo\nplay\n"
                                    "move Pippo 6, 6\nmove Pippo 6, 6\nmove Pippo 6, 6\n"
                                    "move Pippo 6, 6\nmove Pippo 6, 6\nmove Pippo 1, 2\nexit\n");
        CHECK(contains(output, "Pippo rolls 1, 2. Pippo moves from 60 to 63. Pippo Wins!"));
        CHECK(contains(output, "Bye Bye"));
    }
}
