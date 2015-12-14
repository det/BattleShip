#ifndef BATTLESHIP_BOARD_H
#define BATTLESHIP_BOARD_H

#include "ship.hpp"

#include <cassert>
#include <vector>

namespace battleship {

// Represents the state of an arena. Knows which cells contain a ship, what ship
// they contain, how many hits they have left, and which cells have been
// attacked.
class Board {
 private:
  struct ShipCounter {
    Ship ship;
    std::size_t hits_left;
  };

  // Our cell size.
  std::size_t x_size_;
  std::size_t y_size_;

  // Maps a cell to an index into ships_counters.
  std::vector<std::size_t> indexes_;
  // A collection of all ships and how many hits before they are sunk.
  std::vector<ShipCounter> ship_counters_;
  // Maps a cell to if it contains a ship.
  std::vector<bool> ship_map_;
  // Maps a cell to if it has already been attacked.
  std::vector<bool> attacks_;

  std::size_t IndexOf(std::size_t x, std::size_t y) const;
  std::size_t &GetShipIndex(std::size_t x, std::size_t y);
  ShipCounter &GetShipCounter(std::size_t x, std::size_t y);
  bool DoesContainsShip(std::size_t x, std::size_t y) const;
  void SetContainsShip(std::size_t x, std::size_t y);
  bool IsAttacked(std::size_t x, std::size_t y) const;
  void SetAttacked(std::size_t x, std::size_t y);

 public:
  enum PlaceType { kPlaced, kOverlap };

  struct PlaceResult {
    PlaceType type;
    Ship const *ship;
  };

  enum AttackType { kSunk, kMiss, kHit, kRetry };

  struct AttackResult {
    AttackType type;
    Ship const *ship;
  };

  Board(std::size_t x_size = 0, std::size_t y_size = 0);
  void Init(std::size_t x_size, std::size_t y_size);
  PlaceResult Place(Ship const &ship);
  AttackResult Attack(std::size_t x, std::size_t y);
};

}  // namespace battleship
#endif  // #ifndef BATTLESHIP_BOARD_H
