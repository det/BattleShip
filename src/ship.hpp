#ifndef BATTLESHIP_SHIP_H
#define BATTLESHIP_SHIP_H

#include <cstdlib>
namespace battleship {

struct Ship {
  enum Orientation { kHorizontal, kVertical };
  Orientation orientation;
  std::size_t x;
  std::size_t y;
  std::size_t length;
};
}  // namespace battleship
#endif  // #ifndef BATTLESHIP_SHIP_H
