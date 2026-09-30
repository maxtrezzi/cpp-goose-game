#include <stdexcept>
#include <string>

#include "doctest.h"
#include "core.hpp"

using namespace goose_game::core;

namespace {

  // Moves the player with the same dice values the given number of times
  void moveTimes(Game& game, const std::string& name, int times,
                 Board::size_type firstDice, Board::size_type secondDice) {
      for (int i = 0; i < times; ++i) {
          game.movePlayer(name, firstDice, secondDice);
      }
  }

  Players playersNamed(std::initializer_list<std::string> names) {
      Players players;
      for (const auto& name : names) {
          players.addPlayer(Player(name));
      }
      return players;
  }
}

TEST_CASE("Dice rolls values between 1 and 6") {
    Dice dice;
    for (int i = 0; i < 1000; ++i) {
        auto value = dice.roll();
        REQUIRE(value >= 1);
        REQUIRE(value <= Consts::DICE_FACES);
    }
}

TEST_CASE("Board") {
    Board board {Consts::SPACE_COUNT, Consts::BRIDGES, Consts::GOOSES};

    SUBCASE("has the expected spaces") {
        CHECK(board.getLastIndex() == 63);
        CHECK(board.get(0) == NORMAL);
        CHECK(board.get(6) == BRIDGE);
        CHECK(board.get(5) == GOOSE);
        CHECK(board.get(27) == GOOSE);
        CHECK(board.get(63) == FINISH);
    }

    SUBCASE("a position after the last space is not normal") {
        CHECK(board.isNormalPosition(1));
        CHECK_FALSE(board.isNormalPosition(6));
        CHECK_FALSE(board.isNormalPosition(64));
    }

    SUBCASE("rejects special spaces outside the board") {
        CHECK_THROWS_AS(Board(10, {10}, {}), std::invalid_argument);
        CHECK_THROWS_AS(Board(10, {}, {11}), std::invalid_argument);
    }
}

TEST_CASE("Players") {
    Players players;
    CHECK(players.isEmpty());

    players.addPlayer(Player("Pippo"));
    CHECK_FALSE(players.isEmpty());
    CHECK(players.hasPlayer(Player("Pippo")));
    CHECK_FALSE(players.hasPlayer(Player("Pluto")));
    CHECK(players.getAllPlayersAsString() == "Pippo");

    CHECK_THROWS_WITH_AS(players.addPlayer(Player("Pippo")),
                         "Pippo: already existing player\n", std::invalid_argument);
}

TEST_CASE("App adds players") {
    App app;
    CHECK(app.addPlayer("Pippo") == "Player Pippo successfully added\n");
    CHECK(app.addPlayer("Pippo") == "Pippo: already existing player\n");
    CHECK(app.addPlayer("") == "Player's name is required\n");
    CHECK(app.getPlayers().hasPlayer(Player("Pippo")));
    CHECK(app.createNewGame() != nullptr);
}

TEST_CASE("Game moves") {
    Players players = playersNamed({"Pippo", "Pluto"});
    Game game(players);

    SUBCASE("simple moves") {
        CHECK(game.movePlayer("Pippo", 4, 3) == "Pippo rolls 4, 3. Pippo moves from Start to 7");
        CHECK(game.movePlayer("Pluto", 2, 2) == "Pluto rolls 2, 2. Pluto moves from Start to 4");
        CHECK(game.movePlayer("Pippo", 2, 3) == "Pippo rolls 2, 3. Pippo moves from 7 to 12");
    }

    SUBCASE("unknown player") {
        CHECK(game.movePlayer("Paperino", 1, 1) == "Unknown player Paperino\n");
    }

    SUBCASE("the bridge") {
        CHECK(game.movePlayer("Pippo", 4, 2)
              == "Pippo rolls 4, 2. Pippo moves from Start to The Bridge. Pippo jumps to 12");
    }

    SUBCASE("single jump on the goose") {
        game.movePlayer("Pippo", 1, 2);
        CHECK(game.movePlayer("Pippo", 1, 1)
              == "Pippo rolls 1, 1. Pippo moves from 3 to 5, The Goose. Pippo moves again and goes to 7");
    }

    SUBCASE("multiple jumps on the goose") {
        game.movePlayer("Pippo", 4, 6);
        CHECK(game.movePlayer("Pippo", 2, 2)
              == "Pippo rolls 2, 2. Pippo moves from 10 to 14, The Goose. "
                 "Pippo moves again and goes to 18, The Goose. Pippo moves again and goes to 22");
    }

    SUBCASE("prank") {
        moveTimes(game, "Pippo", 1, 6, 6);
        game.movePlayer("Pippo", 1, 2);
        moveTimes(game, "Pluto", 1, 6, 6);
        CHECK(game.movePlayer("Pluto", 1, 2)
              == "Pluto rolls 1, 2. Pluto moves from 12 to 15. On 15 there is Pippo, who returns to 12");
        CHECK(game.movePlayer("Pippo", 1, 1) == "Pippo rolls 1, 1. Pippo moves from 12 to 14, The Goose. "
                                                "Pippo moves again and goes to 16");
    }

    SUBCASE("bounce") {
        moveTimes(game, "Pippo", 5, 6, 6);
        CHECK(game.movePlayer("Pippo", 3, 2)
              == "Pippo rolls 3, 2. Pippo moves from 60 to 63. Pippo bounces! Pippo returns to 61");
        CHECK_FALSE(game.hasWinner());
    }

    SUBCASE("victory") {
        moveTimes(game, "Pippo", 5, 6, 6);
        CHECK_FALSE(game.hasWinner());
        CHECK(game.movePlayer("Pippo", 1, 2) == "Pippo rolls 1, 2. Pippo moves from 60 to 63. Pippo Wins!\n");
        REQUIRE(game.hasWinner());
        CHECK(game.getWinner().getPlayer()->getName() == "Pippo");
    }

    SUBCASE("moving with random dice") {
        auto message = game.moveThrowingDice("Pippo");
        CHECK(message.rfind("Pippo rolls ", 0) == 0);
    }
}
