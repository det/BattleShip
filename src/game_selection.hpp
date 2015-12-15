#ifndef BATTLESHIP_GAME_SELECTION_H
#define BATTLESHIP_GAME_SELECTION_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QRadioButton>

namespace battleship {

struct MapSize {
  std::size_t x;
  std::size_t y;
};

struct ShipSet {
  std::size_t* first;
  std::size_t* last;
};

// Modal dialog for creating a new game.
class GameSelection : public QDialog {
  Q_OBJECT

 private:
  QRadioButton* size_radio1_;
  QRadioButton* size_radio2_;
  QRadioButton* size_radio3_;
  QRadioButton* size_radio4_;
  QRadioButton* set_radio1_;
  QRadioButton* set_radio2_;
  QRadioButton* set_radio3_;
  QRadioButton* set_radio4_;
  QDialogButtonBox* buttons_;

 public:
  GameSelection(QWidget* parent);
  QDialogButtonBox* buttons();
  MapSize GetMapSize();
  ShipSet GetShipSet();
};

}  // namespace battleship
#endif  // #ifndef BATTLESHIP_GAME_SELECTION_H
