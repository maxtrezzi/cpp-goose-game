#include <stdexcept>
#include <unordered_set>
#include <string>
#include <algorithm>

#include "mt.hpp"
#include "core.hpp"

using namespace std;

namespace goose_game {
  namespace core {

    /**
      * Dice class
      */
    Dice::Dice() : rd{}, mt{rd()}, dist{1, Consts::DICE_FACES} {
    }


    /**
      *  Board class
      */
    Board::Board(const size_type size, const SpaceIndexesVector& bridges, const SpaceIndexesVector& gooses) {
        spaces.resize(size, NORMAL);

        for (size_type index: bridges) {
            if ( index < size ) {
                spaces[index] = BRIDGE;
            } else {
                throw invalid_argument("Bridges in Board constructor");
            }
        }

        for (size_type index: gooses) {
            if ( index < size ) {
                spaces[index] = GOOSE;
            } else {
                throw invalid_argument("Gooses in Board constructor");
            }
        }

        spaces[getLastIndex()] = FINISH;
    }

    bool Board::isNormalPosition(const size_type position) const {
        if (position > getLastIndex()) {
            return false;
        } else {
            return (get(position) == NORMAL);
        }
    }


    /*
     * Player class
     */
    Player::Player(const std::string& name) : name(name) {
    }

    bool operator == (const Player& left, const Player& right) {
        return (left.getName() == right.getName());
    }

    /*
     * Players class
     */

    Players& Players::addPlayer(const Player& player) {
        if (!hasPlayer(player)) {
            collection.insert(player);
        } else {
            throw invalid_argument(mt::string_format(Messages::ALREADY_EXISTING_PLAYER, player.getName().c_str()));
        }
        return *this;
    }

    std::string Players::getAllPlayersAsString() const {
        std::string comma = "";
        std::string result = "";

        for (const auto& p : collection) {
            result.append(comma);
            result.append(p.getName());
            comma = ", ";
        }

        return result;
    }

    /**
      * GamePlayer
      */
    GamePlayer::GamePlayer(Game* game, const Player* player) : player(player), game(game), position {0} {
    }


    std::string GamePlayer::moveBy(const Board::size_type firstDice, const Board::size_type secondDice) {
        assert ( (firstDice > 0) && (firstDice <= Consts::DICE_FACES) );
        assert ( (secondDice > 0) && (secondDice <= Consts::DICE_FACES) );
        auto newPosition = position + firstDice + secondDice;
        const Board& board = game->getBoard();
        std::string message;
        message.append(mt::string_format(Messages::PLAYER_ROLLS, getName(), firstDice, secondDice));
        message.append(getTextForTargetSpace(newPosition, false));
        while ((!board.isNormalPosition(newPosition) && (!game->hasWinner()))) {
            if (board.getLastIndex() >= newPosition) {
                auto spaceType = board.get(newPosition);

                if (spaceType == GOOSE) {
                    newPosition += firstDice + secondDice;
                    message.append(Messages::THE_GOOSE)
                           .append(getTextForTargetSpace(newPosition, true));
                } else if (spaceType == BRIDGE) {
                    newPosition += Consts::BRIDGE_SPACES_TO_ADVANCE;
                    message.append(mt::string_format(Messages::PLAYER_JUMPS_TO, getName(), newPosition));
                } else {
                    game->setWinner(*this);
                    message.append(mt::string_format(Messages::PLAYER_WINS, getName()));
                }
            } else {
                newPosition = board.getLastIndex() - (newPosition - board.getLastIndex());
                message.append(mt::string_format(Messages::PLAYER_BOUNCE_TO, getName(), newPosition));
            }
        }
        message.append(processPrank(position, newPosition));
        position = newPosition;
        return message;
    }

    std::string GamePlayer::processPrank(const Board::size_type oldPosition, const Board::size_type newPosition) {
        if (newPosition != position) {
            GamePlayer* colliding = game->findPlayerOnSpace(newPosition, this);

            if (colliding != nullptr) {
                colliding->forceMove(oldPosition);
                return mt::string_format(Messages::PRANK, newPosition,
                        colliding->getName(), getPositionAsString(oldPosition).c_str());
            }
        }

        return "";
    }

    std::string GamePlayer::getTextForTargetSpace(const Board::size_type newPosition, const bool again) {
        const Board& board = game->getBoard();
        auto index = std::min(board.getLastIndex(), newPosition);
        if (board.get(index) == BRIDGE) {
            return mt::string_format(Messages::PLAYER_MOVES_TO_THE_BRIDGE, getName(), getPositionAsString(position).c_str());
        } else if (again) {
            return mt::string_format(Messages::PLAYER_MOVES_AGAIN_TO, getName(), index);
        } else {
            return mt::string_format(Messages::PLAYER_MOVES_FROM_TO, getName(), getPositionAsString(position).c_str(), index);
        }
    }


    /**
     *  GamePlayers
     */
    GamePlayers::GamePlayers (Game* game, const Players& players) : game {game} {
        for (const auto& player : players.getAll()) {
            gamePlayers.insert(collection_type::value_type(player.getName(),
              GamePlayer{game,&player} ) );
        }
    }

    GamePlayer* GamePlayers::getPlayerByName(const std::string& name) {
      auto iter = gamePlayers.find(name);
      return (iter != gamePlayers.end()) ? &iter->second : nullptr;
    }

    GamePlayer* GamePlayers::findPlayerOnSpace (const Board::size_type space, const GamePlayer* playerToExclude) {
        for (auto& pair : gamePlayers) {
            if ( (&pair.second != playerToExclude) && (pair.second.getPosition()==space) ) {
                return &pair.second;
            }
        }
        return nullptr;
    }

    /**
      * Game
      */
    Game::Game(const Players& players) : board {Consts::SPACE_COUNT, Consts::BRIDGES, Consts::GOOSES}, players(this, players) {
    }

    std::string Game::movePlayer(const std::string& name, Board::size_type firstDice, Board::size_type secondDice) {
      assert ( !name.empty() );
      assert ( (firstDice > 0) && (firstDice <= Consts::DICE_FACES) );
      assert ( (secondDice > 0) && (secondDice <= Consts::DICE_FACES) );
      GamePlayer* player = players.getPlayerByName(name);
      if (player != nullptr) {
          return player->moveBy(firstDice, secondDice);
      } else {
          return mt::string_format(Messages::UNKNOWN_PLAYER, name.c_str());
      }
    }

    std::string Game::moveThrowingDice(const std::string& playerName) {
      return movePlayer(playerName, dice1.roll(), dice2.roll());
    }
  } // core
} // goose_game
