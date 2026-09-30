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
        virtual ~View() = default;
        virtual View* show()=0;
    };

    class MoveArgs {
       public:
         MoveArgs(int firstDice, int secondDice, const std::string& playerName);

         inline bool isComplete() const {
            return (secondDice != 0);
         }

         inline int getFirstDice() const {
            return firstDice;
         }

         inline int getSecondDice() const {
            return secondDice;
         }

         inline const std::string& getPlayerName() const {
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
        explicit GameView(core::Game* game);
        View* show() override;
    };


    class AppView : public View {
      private:
        core::App app_model;

        AppView* startNewGame();
      public:
        AppView();
        View* show() override;
    };
  }
}

#endif
