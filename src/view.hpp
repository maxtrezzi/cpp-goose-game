#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <iostream>

#include "core.hpp"

namespace goose_game {
  namespace view {


    class View {
      protected:
        View* println(const std::string &line);
      public:
        virtual View* show()=0;
    };

    class MoveArgs {
       public:
         MoveArgs(int firstDice, int secondDice, const std::string& playerName);

         inline bool isComplete() {
            return (secondDice != 0);
         }

         inline int getFirstDice() {
            return firstDice;
         }

         inline int getSecondDice() {
            return secondDice;
         }

         inline const std::string& getPlayerName() {
            return playerName;
         }

        static MoveArgs parseMoveArgs(const std::string& string);
      private:
        int firstDice;
        int secondDice;
        std::string playerName;
    };


    class GameView : public View {
      private:
        core::Game* game;
      public:
        GameView(core::Game* game);
        virtual View* show();
    };


    class AppView : public View {
      private:
        core::App app_model;

        AppView* startNewGame();
      public:
        AppView();
        virtual View* show();
    };
  }
}

#endif
