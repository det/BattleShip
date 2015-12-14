#ifndef BATTLESHIP_GAME_H
#define BATTLESHIP_GAME_H

#include "arena.hpp"
#include "board.hpp"
#include "game_selection.hpp"
#include "rules.hpp"

#include <bitset>
#include <QEvent>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>

namespace battleship {

// Ties together an 2 Arenas and 2 Boards. Implements game logic.
class Game : public QMainWindow {
  Q_OBJECT

 private:
  struct Group {
    Arena *arena;
    Board board;
    std::size_t ships_left;
    std::vector<std::size_t> ship_set;
  };

  NewGameSelector game_selection_;
  Rules rules_;
  QMenuBar *menu_bar_;
  QHBoxLayout *layout_;
  QStatusBar *status_bar_;
  Group group1_;
  Group group2_;

  void BeginPlacing1();
  void BeginPlacing2();
  void BeginAttacking1();
  void BeginAttacking2();

  bool PlaceShip(Group &group, Ship const &ship);
  bool Attack(Group &group, std::size_t x, std::size_t y);

  void HandleShipPlaced1(Ship const &ship);
  void HandleAttacked1(std::size_t x, std::size_t y);
  void HandleShipPlaced2(Ship const &ship);
  void HandleAttacked2(std::size_t x, std::size_t y);
  void HandleNewGame(bool);
  void HandleExit(bool);
  void HandleHowToPlay(bool);
  void HandleCreateNewGame();
  void HandleCancelNewGame();

 protected:
  bool event(QEvent *event);

 public:
  Game(std::size_t width = 10, std::size_t height = 10);
};

}  // namespace battleship
#endif  // #ifndef BATTLESHIP_GAME_H
