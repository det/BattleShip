#include "arena.hpp"

#include <QMouseEvent>
#include <QPainter>

#include <iostream>

namespace battleship {

// Size constants.
static int const kCellSize = 30;
static int const kAttackSize = 12;
static int const kAttackMargin = (kCellSize - kAttackSize) / 2;
static int const kMaxWidth = 26;
static int const kMaxHeight = 26;

// Grid labels.
static char const *const x_labels_[kMaxWidth] = {
    "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",
    "10", "11", "12", "13", "14", "15", "16", "17", "18",
    "19", "20", "21", "22", "23", "24", "25", "26"};

static char const *const y_labels_[kMaxHeight] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

// Color constants.
static int const kShipColor[] = {160, 160, 160};
static int const kMissColor[] = {255, 255, 255};
static int const kHitColor[] = {255, 0, 0};
static int const kSeaColor[] = {224, 224, 255};
static int const kFocusColor[] = {0, 0, 255};
static int const kDragColor[] = {192, 192, 224};

struct ShipOption {
  bool is_valid;
  Ship ship;
};

// Return a Ship if the pair of grid numbers aligned with the grid.
static ShipOption MakeShip(std::size_t pressed_x, std::size_t pressed_y,
                           std::size_t x, std::size_t y) {
  ShipOption option;
  if (x == pressed_x) {
    option.ship.orientation = Ship::kVertical;
    if (y < pressed_y) {
      option.ship.x = x;
      option.ship.y = y;
      option.ship.length = pressed_y - y + 1;
    } else {
      option.ship.x = x;
      option.ship.y = pressed_y;
      option.ship.length = y - pressed_y + 1;
    }
  } else if (y == pressed_y) {
    option.ship.orientation = Ship::kHorizontal;
    if (x < pressed_x) {
      option.ship.x = x;
      option.ship.y = y;
      option.ship.length = pressed_x - x + 1;
    } else {
      option.ship.x = pressed_x;
      option.ship.y = y;
      option.ship.length = x - pressed_x + 1;
    }
  } else {
    option.is_valid = false;
    return option;
  }

  option.is_valid = true;
  return option;
}

// Check for valid grid numbers.
bool Arena::CheckBounds(int x, int y) {
  if (x < 0 || x >= x_size_ || y < 0 || y >= y_size_) return false;
  return true;
}

// Convert screen coordinates to grid numbers.
int Arena::GetCellFromPosition(int pos) {
  // The grid is offset by 1 because of the labels
  return pos / kCellSize - 1;
}

// Draw the arena according to mode.
void Arena::paintEvent(QPaintEvent *) {
  DrawGrid();

  switch (mode_) {
    case kPlace:
      DrawDrag();
    case kReveal:
      DrawReveal();
      break;

    case kAttack:
    case kDisplay:
      DrawSunk();
      break;
  }

  switch (mode_) {
    case kPlace:
    case kAttack:
      DrawFocus();
      break;
    case kReveal:
    case kDisplay:
      break;
  }

  DrawMisses();
  DrawHits();
}

// Handle a button press.
void Arena::mousePressEvent(QMouseEvent *event) {
  // Check if we are in the map and convert to grid numbers.
  int x = GetCellFromPosition(event->x());
  int y = GetCellFromPosition(event->y());
  if (!CheckBounds(x, y)) return;
  std::size_t x2 = static_cast<std::size_t>(x);
  std::size_t y2 = static_cast<std::size_t>(y);

  // Draw according to mode.
  switch (mode_) {
    case kDisplay:
    case kReveal:
      return;

    case kPlace:
      // Mark the current cell as selected.
      drag_rect_ = MakeSingleRect(x2, y2);
      this->update();
    case kAttack:
      pressed_x_ = x2;
      pressed_y_ = y2;
      break;
  }
}

// Handle a button release.
void Arena::mouseReleaseEvent(QMouseEvent *event) {
  // We don't have a selection anymore.
  drag_rect_ = QRect();
  this->update();

  // Check if we are in the map and convert to grid numbers.
  int x = GetCellFromPosition(event->x());
  int y = GetCellFromPosition(event->y());
  if (!CheckBounds(x, y)) return;
  std::size_t x2 = static_cast<std::size_t>(x);
  std::size_t y2 = static_cast<std::size_t>(y);

  // Emit events according to mode.
  switch (mode_) {
    case kDisplay:
    case kReveal:
      break;

    case kAttack:
      if (x2 == pressed_x_ && y2 == pressed_y_) emit Attacked(x2, y2);
      break;

    case kPlace:
      ShipOption option = MakeShip(pressed_x_, pressed_y_, x2, y2);
      if (option.is_valid) emit ShipPlaced(option.ship);
      break;
  }
}

// Handle mouse movement while a button is clicked.
void Arena::mouseMoveEvent(QMouseEvent *event) {
  // Check if we are in the map and convert to grid numbers.
  int x = GetCellFromPosition(event->x());
  int y = GetCellFromPosition(event->y());
  if (!CheckBounds(x, y)) {
    // We don't have a selection anymore.
    drag_rect_ = QRect();
    this->update();
    return;
  }
  std::size_t x2 = static_cast<std::size_t>(x);
  std::size_t y2 = static_cast<std::size_t>(y);

  // Update the selection if it changed.
  ShipOption option = MakeShip(pressed_x_, pressed_y_, x2, y2);
  if (option.is_valid) {
    QRect rect = MakeShipRect(option.ship);
    if (rect == drag_rect_) return;
    drag_rect_ = rect;
  } else
    drag_rect_ = QRect();
  this->update();
}

void Arena::DrawGrid() {
  QPainter painter(this);
  // We need an extra cell for the header.
  int num_cell_x = x_size_ + 1;
  int num_cell_y = y_size_ + 1;

  // Draw the background.
  QBrush brush;
  brush.setColor(QColor(kSeaColor[0], kSeaColor[1], kSeaColor[2]));
  brush.setStyle(Qt::SolidPattern);
  painter.fillRect(0, 0, num_cell_x * kCellSize, num_cell_y * kCellSize, brush);

  // We need 1 more lines than cells.
  int num_lines_x = num_cell_x + 1;
  int num_lines_y = num_cell_y + 1;

  // Draw the lines on the x-axis.
  for (int x = 0; x != num_lines_x; ++x) {
    int start_x = x * kCellSize;
    int end_y = num_cell_y * kCellSize;
    painter.drawLine(start_x, 0, start_x, end_y);
  }

  // Draw the lines on the y-axis.
  for (int y = 0; y != num_lines_y; ++y) {
    int start_y = y * kCellSize;
    int end_x = num_cell_x * kCellSize;
    painter.drawLine(0, start_y, end_x, start_y);
  }

  QFontMetrics fm(painter.font());

  // Label the x-axis using the x table.
  for (int x = 1; x != num_cell_x; ++x) {
    // Draw the label centered in the cell.
    char const *label = x_labels_[x - 1];
    int char_width = fm.width(label);
    int char_height = fm.height();
    int x_pos = x * kCellSize + (kCellSize - char_width) / 2;
    int y_pos = (kCellSize + char_height) / 2;
    painter.drawText(x_pos, y_pos, label);
  }

  // Label the y-axis using the y table.
  for (int y = 1; y != num_cell_y; ++y) {
    // Draw the label centered in the cell.
    char const *label = y_labels_[y - 1];
    int char_width = fm.width(label);
    int char_height = fm.height();
    int x_pos = (kCellSize - char_width) / 2;
    int y_pos = y * kCellSize + (kCellSize + char_height) / 2;
    painter.drawText(x_pos, y_pos, label);
  }
}

// Draw the arena as accepting input.
void Arena::DrawFocus() {
  QPainter painter(this);
  QBrush brush;
  brush.setStyle(Qt::NoBrush);
  painter.setBrush(brush);
  QPen pen;
  pen.setColor(QColor(kFocusColor[0], kFocusColor[1], kFocusColor[2]));
  pen.setWidth(4);
  painter.setPen(pen);
  QRect rect;
  rect.setLeft(1);
  rect.setTop(1);
  rect.setRight(kCellSize * x_size_ + kCellSize);
  rect.setBottom(kCellSize * y_size_ + kCellSize);
  painter.drawRect(rect);
}

// Draw all sunk ships.
void Arena::DrawSunk() {
  QPainter painter(this);
  QBrush brush;
  brush.setColor(QColor(kShipColor[0], kShipColor[1], kShipColor[2]));
  brush.setStyle(Qt::SolidPattern);
  painter.setBrush(brush);

  for (std::size_t i = 0, e = sunk_rects_.size(); i != e; ++i)
    painter.drawRect(sunk_rects_[i]);
}

// Draw all revealed ships.
void Arena::DrawReveal() {
  QPainter painter(this);
  QBrush brush;
  brush.setColor(QColor(kShipColor[0], kShipColor[1], kShipColor[2]));
  brush.setStyle(Qt::SolidPattern);
  painter.setBrush(brush);

  for (std::size_t i = 0, e = reveal_rects_.size(); i != e; ++i)
    painter.drawRect(reveal_rects_[i]);
}

// Draw the current ship placement that is being dragged.
void Arena::DrawDrag() {
  QPainter painter(this);
  QBrush brush;
  brush.setColor(QColor(kDragColor[0], kDragColor[1], kDragColor[2]));
  brush.setStyle(Qt::SolidPattern);
  painter.setBrush(brush);
  painter.drawRect(drag_rect_);
}

// Draw all hit attacks.
void Arena::DrawHits() {
  QPainter painter(this);
  QBrush brush;
  brush.setColor(QColor(kHitColor[0], kHitColor[1], kHitColor[2]));
  brush.setStyle(Qt::SolidPattern);
  painter.setBrush(brush);

  for (std::size_t i = 0, e = hit_rects_.size(); i != e; ++i)
    painter.drawEllipse(hit_rects_[i]);
}

// Draw all missed attacks.
void Arena::DrawMisses() {
  QPainter painter(this);
  QBrush brush;
  brush.setColor(QColor(kMissColor[0], kMissColor[1], kMissColor[2]));
  brush.setStyle(Qt::SolidPattern);
  painter.setBrush(brush);

  for (std::size_t i = 0, e = miss_rects_.size(); i != e; ++i)
    painter.drawEllipse(miss_rects_[i]);
}

// Returns A rect covering an entire ship.
QRect Arena::MakeShipRect(Ship const &ship) {
  // The grid is offset by 1 because of the labels.
  int x = static_cast<int>(ship.x + 1);
  int y = static_cast<int>(ship.y + 1);
  int length2 = static_cast<int>(ship.length);

  QRect rect;
  rect.setLeft(x * kCellSize);
  rect.setTop(y * kCellSize);

  switch (ship.orientation) {
    case Ship::kHorizontal:
      rect.setWidth(kCellSize * length2);
      rect.setHeight(kCellSize);
      break;

    case Ship::kVertical:
      rect.setWidth(kCellSize);
      rect.setHeight(kCellSize * length2);
      break;
  }

  return rect;
}

// Returns A rect covering the part of a cell to draw the attack on.
QRect Arena::MakeAttackRect(std::size_t x, std::size_t y) {
  // The grid is offset by 1 because of the labels
  int x2 = static_cast<int>(x + 1);
  int y2 = static_cast<int>(y + 1);

  QRect rect;
  rect.setLeft(x2 * kCellSize + kAttackMargin);
  rect.setTop(y2 * kCellSize + kAttackMargin);
  rect.setWidth(kAttackSize);
  rect.setHeight(kAttackSize);
  return rect;
}

// Returns A rect covering a single cell.
QRect Arena::MakeSingleRect(std::size_t x, std::size_t y) {
  // The grid is offset by 1 because of the labels.
  int x2 = static_cast<int>(x + 1);
  int y2 = static_cast<int>(y + 1);

  QRect rect;
  rect.setLeft(x2 * kCellSize);
  rect.setTop(y2 * kCellSize);
  rect.setWidth(kCellSize);
  rect.setHeight(kCellSize);
  return rect;
}

// Make the arena accept new ships.
void Arena::SetPlacing() {
  mode_ = kPlace;
  this->update();
}

// Make the arena accept attacks.
void Arena::SetAttacking() {
  mode_ = kAttack;
  this->update();
}

// Make the arena display Attacks and sunk ships.
void Arena::SetDisplaying() {
  mode_ = kDisplay;
  this->update();
}

// Make the arena display Attacks and revealed ships.
void Arena::SetRevealing() {
  mode_ = kReveal;
  this->update();
}

// Add a revealed ship.
void Arena::AddReveal(Ship const &ship) {
  reveal_rects_.push_back(MakeShipRect(ship));
  this->update();
}

// Add a sunk ship.
void Arena::AddSunk(const Ship &ship) {
  sunk_rects_.push_back(MakeShipRect(ship));
  this->update();
}

// Add a hit attack.
void Arena::AddHit(std::size_t x, std::size_t y) {
  hit_rects_.push_back(MakeAttackRect(x, y));
  this->update();
}

// Add a missed attack.
void Arena::AddMiss(std::size_t x, std::size_t y) {
  miss_rects_.push_back(MakeAttackRect(x, y));
  this->update();
}

// Add construct an x_size by y_size board.
Arena::Arena(std::size_t x_size, std::size_t y_size) { Init(x_size, y_size); }

// Resize the board and clear all state.
void Arena::Init(std::size_t x_size, std::size_t y_size) {
  mode_ = kDisplay;
  x_size_ = static_cast<int>(x_size);
  y_size_ = static_cast<int>(y_size);
  sunk_rects_.clear();
  reveal_rects_.clear();
  hit_rects_.clear();
  miss_rects_.clear();
  updateGeometry();
}

// Return our optimal size.
QSize Arena::sizeHint() const {
  QSize size;
  size.setWidth(x_size_ * kCellSize + kCellSize + 2);
  size.setHeight(y_size_ * kCellSize + kCellSize + 4);
  return size;
}

}  // namespace battleship
