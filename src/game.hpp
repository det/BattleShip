#ifndef BATTLESHIP_GAME_H
#define BATTLESHIP_GAME_H

#include "arena.hpp"
#include "board.hpp"

#include <bitset>
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
    std::bitset<5> ships_placed;
  };

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
  void HandleNewGame(bool b);
  void HandleHowToPlay(bool b);

 public:
  Game();
};
}  // namespace battleship
#endif  // #ifndef BATTLESHIP_GAME_H
