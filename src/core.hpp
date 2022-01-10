#ifndef CORE_H
#define CORE_H

#include <cassert>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <random>
#include <string>
#include <utility>

#include "mt.hpp"

namespace goose_game {
  namespace core {

    class Dice {
        public:
            Dice();

            Dice(Dice&& other);

            inline unsigned int roll() {
                return dist(mt);
            };
        private:
            std::random_device rd;
            std::mt19937 mt;
            std::uniform_int_distribution<int> dist;
    };

    enum SpaceType {
        NORMAL,
        BRIDGE,
        GOOSE,
        FINISH
    };

    typedef std::vector<SpaceType> SpaceTypeVector;
    typedef std::vector<SpaceTypeVector::size_type> SpaceIndexesVector;

    class Board  {
        public:
            typedef SpaceTypeVector::size_type size_type;

            Board(const size_type size, const SpaceIndexesVector bridges, const SpaceIndexesVector gooses);

            inline SpaceType get(const size_type position) const {
                //here I know that size_type cannot be negative, but is it right to assume this here?
                assert ( position <= spaces.size() );

                return spaces[position];
            }

            inline size_type getLastIndex() const {
                return spaces.size()-1;
            }

            bool isNormalPosition(const size_type position) const;
        private:
            std::vector<SpaceType> spaces;
    };

    class Consts final {
      public:
        static const Board::size_type BRIDGE_SPACES_TO_ADVANCE = 6;
        static const Board::size_type SPACE_COUNT = 64;
        static inline const SpaceIndexesVector BRIDGES = {6};
        static inline const SpaceIndexesVector GOOSES = {5,9,14,18,23,27};
        static inline const std::string ADD_PLAYER_COMMAND = "add player";
        static inline const std::string EXIT_COMMAND = "exit";
        static inline const std::string PLAY_COMMAND = "play";
        static inline const std::string MOVE_PLAYER_COMMAND = "move";
    };

      class Messages final {
      public:
        static inline const std::string APP_MENU =
                "\n"
                "==Goose Game App Commands==\n"
                " add player <player-name>\n"
                " play\n"
                " exit\n"
                "Please input your command";

        static inline const std::string GAME_MENU =
                "\n"
                "============ Game Commands ============\n"
                " move <player-name> [<dice1>,<dice2>]\n"
                " exit\n"
                "Please input your command ";

        static inline const std::string PLAYER_ADDED = "Player %s successfully added\n";
        static inline const std::string ADD_PLAYER_ERROR = "Error while adding new player %s:\n%s\n";
        static inline const std::string ALREADY_EXISTING_PLAYER = "%s: already existing player\n";
        static inline const std::string PLAYERS = "players: %s\n";
        static inline const std::string UNKNOWN_COMMAND = "Unknown command\n";
        static inline const std::string UNKNOWN_PLAYER = "Unknown player %s\n";
        static inline const std::string PLAYER_NAME_IS_REQUIRED = "Player's name is required\n";
        static inline const std::string BYE = "Bye Bye\n";
        static inline const std::string GAME_QUITTED = "Game quitted\n";
        static inline const std::string NO_PLAYERS = "No players for the game\n";
        static inline const std::string INVALID_DICE_ARG = "Invalid dice argument: %s\n";
        static inline const std::string START = "Start";
        static inline const std::string PLAYER_MOVES_FROM_TO = "%s moves from %s to %d";
        static inline const std::string PLAYER_MOVES_AGAIN_TO = ". %s moves again and goes to %d";
        static inline const std::string PLAYER_JUMPS_TO = ". %s jumps to %d";
        static inline const std::string PLAYER_MOVES_TO_THE_BRIDGE = "%s moves from %s to The Bridge";
        static inline const std::string PLAYER_BOUNCE_TO = ". %1$s bounces! %1$s returns to %2$d";
        static inline const std::string PLAYER_ROLLS = "%s rolls %d, %d. ";
        static inline const std::string PLAYER_WINS = ". %s Wins!\n";
        static inline const std::string MOVE_PLAYER_NAME_IS_REQUIRED = "Command Move: Player's name is required\n";
        static inline const std::string MOVE_PLAYER_INVALID_ARGS = "Command Move: Invalid arguments\n";
        static inline const std::string PRANK = ". On %d there is %s, who returns to %s";
        static inline const std::string THE_GOOSE = ", The Goose";
    }; //Messages

    class Player final {
      public:
        explicit Player(const std::string& name);

        inline const std::string& getName() const {
            return name;
        };

        inline bool operator <(const Player &b) const {
          return name < b.name;
        };
      private:
        std::string name;
    };

    bool operator == (const Player& left, const Player& right);

    struct PlayerHashFunction {
      std::size_t operator()(const Player& p) const {
        std::hash<std::string> hashVal;
        return hashVal(p.getName());
      }
    };

    class Players  {
      public:
        typedef std::unordered_set<Player, PlayerHashFunction> collection_type;

        Players& addPlayer(const Player& player);

        inline bool isEmpty() const {
          return collection.empty();
        };

        inline bool hasPlayer (const Player& player) const {
          return (collection.count(player) == 1);
        };

        inline auto getAll() const {
          return mt::const_range<collection_type>(&collection);
        };

        std::string getAllPlayersAsString() const;
      private:
        collection_type collection;
    };


    class Game;
    class GamePlayers;

    class GamePlayer {
        public:
            GamePlayer(Game* game, const Player* player);

            std::string moveBy(const Board::size_type firstDice, const Board::size_type secondDice);

            inline Board::size_type getPosition() const {
                return position;
            }

            inline const Player* getPlayer() const {
                return player;
            }

        private:

            inline GamePlayer& forceMove(const Board::size_type newPos) {
                position = newPos;
                return *this;
            }

            inline std::string getPositionAsString(const Board::size_type position) const {
                return (position > 0) ? std::to_string(position) : Messages::START;
            }

            std::string processPrank(const Board::size_type oldPosition, const Board::size_type newPosition);
            std::string getTextForTargetSpace(const Board::size_type newPosition, const bool again);

            const Player* player;
            Game* game;
            Board::size_type position = 0;
    };

    class GamePlayers {
        public:
            GamePlayers (Game* game, const Players& players);

            inline Game* getGame() const {
                return game;
            }

            GamePlayer* getPlayerByName(const std::string& name);
            GamePlayer* findPlayerOnSpace(Board::size_type space, const GamePlayer* playerToExclude);
        private:
            Game* game;

            typedef std::unordered_map<std::string, GamePlayer> collection_type;

            collection_type gamePlayers;
    };

    class Game {
        public:
            explicit Game(const Players& players);

            Game(Game&& other);

            inline GamePlayer* findPlayerOnSpace(Board::size_type space, const GamePlayer* playerToExclude) {
                return players.findPlayerOnSpace(space, playerToExclude);
            };

            std::string movePlayer(const std::string& name, Board::size_type firstDice, Board::size_type secondDice);
            std::string moveThrowingDice(const std::string& playerName);

            inline Board& getBoard() {
                return board;
            }

            inline Game& setWinner(GamePlayer& player) {
                winner = &player;
                return *this;
            }

            inline GamePlayer& getGamePlayer(const std::string& name) {
                return *players.getPlayerByName(name);
            }

            inline const GamePlayer& getWinner() {
                return *winner;
            }

            inline bool hasWinner() {
                bool ret = winner != nullptr; 
                return ret;
            }
        private:
            Board board;
            GamePlayers players;
            GamePlayer* winner;
            Dice dice1;
            Dice dice2;
    };


    class App {
      public:
        inline App() {};
        inline std::string addPlayer(const std::string& name) {
            try {
              players.addPlayer(Player(name));
              return mt::string_format(Messages::PLAYER_ADDED, name.c_str());
            } catch (std::exception& e) {
              return mt::string_format(Messages::ADD_PLAYER_ERROR, name.c_str(), e.what());
            }
        };

        inline Game* createNewGame() {
            return new Game(players);
          };
        inline const Players& getPlayers() const {
            return players;
        };
      private:
        Players players;
    }; // App
  } // namespace core
} // namespace goose_game

#endif //_H
