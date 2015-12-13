#ifndef BATTLESHIP_ARENA_H
#define BATTLESHIP_ARENA_H

#include "ship.hpp"

#include <QWidget>

namespace battleship {

// Graphical representation of the game state and input mechanism for placing
// ships and attacks.
class Arena : public QWidget {
  Q_OBJECT
 protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);

 private:
  enum Mode { kPlace, kAttack, kDisplay, kReveal };

  Mode mode_;
  // Where the last click was.
  std::size_t pressed_x_;
  std::size_t pressed_y_;

  // Our cell sizes.
  int x_size_;
  int y_size_;

  // Rects for drawing.
  std::vector<QRect> sunk_rects_;
  std::vector<QRect> reveal_rects_;
  std::vector<QRect> hit_rects_;
  std::vector<QRect> miss_rects_;
  QRect drag_rect_;

  int GetCellFromPosition(int pos);
  bool CheckBounds(int x, int y);
  QRect MakeShipRect(Ship const &ship);
  QRect MakeAttackRect(std::size_t x, std::size_t y);
  QRect MakeSingleRect(std::size_t x, std::size_t y);
  void DrawGrid();
  void DrawFocus();
  void DrawSunk();
  void DrawReveal();
  void DrawDrag();
  void DrawHits();
  void DrawMisses();

 public:
  Arena(std::size_t x_size = 10, std::size_t y_size = 10);
  void Init(std::size_t x_size, std::size_t y_size);

  void SetPlacing();
  void SetAttacking();
  void SetDisplaying();
  void SetRevealing();

  void AddReveal(Ship const &ship);
  void AddSunk(Ship const &ship);
  void AddHit(std::size_t x, std::size_t y);
  void AddMiss(std::size_t x, std::size_t y);

  QSize sizeHint() const;

 signals:
  void Attacked(std::size_t x, std::size_t y);
  void ShipPlaced(Ship const &ship);
};
}  // namespace battleship
#endif  // #ifndef BATTLESHIP_ARENA_H
