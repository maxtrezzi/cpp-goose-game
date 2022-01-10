#include "view.hpp"
#include "core.hpp"
#include "mt.hpp"
#include <sstream>

using namespace std;
using namespace goose_game::core;

namespace goose_game {
  namespace view {

    /**
    * View
    */
    View* View::println(const string &line) {
      cout << line << endl;
      return this;
    }


    MoveArgs::MoveArgs(int firstDice, int secondDice, const std::string& playerName) :
      firstDice{firstDice}, secondDice{secondDice}, playerName{playerName} {
    }

    MoveArgs MoveArgs::parseMoveArgs(const std::string& string) {
      std::stringstream stream {string};
      std::string cur;
      int countTokens {0}, firstDice {0}, secondDice{0};
      std::string playerName="";

      stream >> cur;
      playerName = cur;
      ++countTokens;

      while (getline(stream, cur, ',')) {
        int i;
        mt::trim(cur);
        try {
            i = std::stoi(cur);
        } catch (std::invalid_argument& e) {
            throw invalid_argument(mt::string_format(Messages::INVALID_DICE_ARG, cur.c_str()));
        }

        if ( (i < 1) || (i > 6) ) {
            throw invalid_argument(mt::string_format(Messages::INVALID_DICE_ARG, cur.c_str()));
        }
        ++countTokens;
        switch (countTokens) {
            case 2 : firstDice = i;
                break;
            case 3 : secondDice = i;
                break;
            default :
              throw invalid_argument(mt::string_format(Messages::INVALID_DICE_ARG, cur.c_str()));
        }
      }

      if ( countTokens == 0) {
          throw invalid_argument(Messages::MOVE_PLAYER_NAME_IS_REQUIRED);
      }
      if ( (countTokens != 1)  && (countTokens != 3) ) {
          throw invalid_argument(Messages::MOVE_PLAYER_INVALID_ARGS);
      }

      return MoveArgs(firstDice, secondDice, playerName);
    }

    /**
    * GameView
    */
    GameView::GameView(core::Game* game) : game {game} {

    };

    View* GameView::show() {
      while (!game->hasWinner()) {
        cout << core::Messages::GAME_MENU;
        std::string input;
        getline(cin, input);
        if (input.find(Consts::MOVE_PLAYER_COMMAND) == 0) {
          std::string args = input.substr(Consts::MOVE_PLAYER_COMMAND.length());

          try {
            MoveArgs moveArgs = MoveArgs::parseMoveArgs(args);
            if (moveArgs.isComplete()) {
                cout << game->movePlayer(moveArgs.getPlayerName(),moveArgs.getFirstDice(),moveArgs.getSecondDice()) << "\n";
            } else {
                cout << game->moveThrowingDice(moveArgs.getPlayerName()) << "\n";
            }
          } catch (exception& e) {
            cout << "error\n" <<  e.what();
          }
        } else if (input == Consts::EXIT_COMMAND) {
          cout << Messages::GAME_QUITTED;
          break;
        } else {
          cout << Messages::UNKNOWN_COMMAND;
        }
      }

      return this;
    }

    /**
     * AppView
     */

    AppView::AppView() {
    };

    AppView* AppView::startNewGame() {
      std::unique_ptr<Game> game(app_model.createNewGame());  
      GameView(game.get()).show();
      return this;
    }

    View* AppView::show() {
      while (true) {
        println(Messages::APP_MENU);
        string input;
        getline(cin, input);

        if (input.find(Consts::ADD_PLAYER_COMMAND) == 0) {
          string player_name = mt::trim_copy(input.substr(Consts::ADD_PLAYER_COMMAND.size()));
          println(app_model.addPlayer(player_name));
        } else if (input == Consts::PLAY_COMMAND) {
          startNewGame();
        } else if (input == Consts::EXIT_COMMAND) {
          println(Messages::BYE);
          break;
        } else {
          println(Messages::UNKNOWN_COMMAND);
        }
      }
      return this;
    }
  }
}
