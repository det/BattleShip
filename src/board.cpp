#include "board.hpp"

namespace battleship {

// Convert 2d coordinates into 1d coordinates.
std::size_t Board::IndexOf(std::size_t x, std::size_t y) const {
  return y * x_size_ + x;
}

// Return a ship index from grid coordinates.
std::size_t &Board::GetShipIndex(std::size_t x, std::size_t y) {
  return indexes_[IndexOf(x, y)];
}

// Return a ship counter from grid coordinates.
Board::ShipCounter &Board::GetShipCounter(std::size_t x, std::size_t y) {
  return ship_counters_[GetShipIndex(x, y)];
}

// Return whether a cell contains a ship.
bool Board::DoesContainsShip(std::size_t x, std::size_t y) const {
  return ship_map_[IndexOf(x, y)];
}

// Marks a cell as containing a ship.
void Board::SetContainsShip(std::size_t x, std::size_t y) {
  ship_map_[IndexOf(x, y)] = true;
}

// Return whether a cell has already been attacked.
bool Board::IsAttacked(std::size_t x, std::size_t y) const {
  return attacks_[IndexOf(x, y)];
}

// Mark a cell as attacked.
void Board::SetAttacked(std::size_t x, std::size_t y) {
  attacks_[IndexOf(x, y)] = true;
}

// Construct a board and initialize to the specified size.
Board::Board(std::size_t x_size, std::size_t y_size) { Init(x_size, y_size); }

// Initialize the board to the specified size.
void Board::Init(std::size_t x_size, std::size_t y_size) {
  x_size_ = x_size;
  y_size_ = y_size;
  indexes_.resize(x_size * y_size);
  ship_map_.assign(x_size * y_size, false);
  attacks_.assign(x_size * y_size, false);
  ship_counters_.clear();
}

// Try to place a ship and return if we were successful or not.
Board::PlaceResult Board::Place(const Ship &ship) {
  PlaceResult result;
  std::size_t index = ship_counters_.size();

  switch (ship.orientation) {
    case Ship::kHorizontal:
      assert(ship.x + ship.length <= x_size_ && ship.y < y_size_);
      // Bail out if this ship overlaps another.
      for (std::size_t i = 0; i != ship.length; ++i) {
        if (DoesContainsShip(ship.x + i, ship.y)) {
          result.type = kOverlap;
          return result;
        }
      }
      // Place ship
      for (std::size_t i = 0; i != ship.length; ++i) {
        GetShipIndex(ship.x + i, ship.y) = index;
        SetContainsShip(ship.x + i, ship.y);
      }
      break;

    case Ship::kVertical:
      assert(ship.y + ship.length <= y_size_ || ship.x < x_size_);
      // Bail out if this ship overlaps another.
      for (std::size_t i = 0; i != ship.length; ++i) {
        if (DoesContainsShip(ship.x, ship.y + i)) {
          result.type = kOverlap;
          return result;
        }
      }

      // Place ship
      for (std::size_t i = 0; i != ship.length; ++i) {
        GetShipIndex(ship.x, ship.y + i) = index;
        SetContainsShip(ship.x, ship.y + i);
      }
      break;
  }

  ShipCounter ship_counter;
  ship_counter.ship = ship;
  ship_counter.hits_left = ship.length;
  ship_counters_.push_back(ship_counter);
  result.type = kPlaced;
  result.ship = &ship_counters_.back().ship;
  return result;
}

// Try to attack a cell and return the status of the cell.
Board::AttackResult Board::Attack(std::size_t x, std::size_t y) {
  assert(x < x_size_ && y < y_size_);

  AttackResult result;

  // Did we already try to attack here?
  if (IsAttacked(x, y)) {
    result.type = kRetry;
    return result;
  }

  // Set the cell as attacked.
  SetAttacked(x, y);

  // Did we miss?
  if (!DoesContainsShip(x, y)) {
    result.type = kMiss;
    return result;
  }

  // We must have hit a ship.
  ShipCounter &counter = GetShipCounter(x, y);
  result.ship = &counter.ship;

  // Did we sink it ?
  --counter.hits_left;
  if (counter.hits_left == 0)
    result.type = kSunk;
  else
    result.type = kHit;

  return result;
}

}  // namespace battleship
