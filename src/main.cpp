#include "game.hpp"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  battleship::Game game;
  game.show();
  return a.exec();
}
