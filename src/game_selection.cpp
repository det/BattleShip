#include "game_selection.hpp"

namespace battleship {

// Options.
static MapSize size1 = {8, 8};
static MapSize size2 = {10, 10};
static MapSize size3 = {9, 16};
static MapSize size4 = {26, 26};
static std::size_t set1[] = {1, 2, 3};
static std::size_t set2[] = {2, 3, 3, 4, 5};
static std::size_t set3[] = {1, 3, 5, 7};
static std::size_t set4[] = {1, 1, 2, 3, 5, 8};

// Return a string representation of a map size.
static QString MakeSizeString(MapSize size) {
  QString ret;
  ret.append(QString::number(size.x));
  ret.append("x");
  ret.append(QString::number(size.y));
  return ret;
}

// Generate an iterator pair from an array.
template <std::size_t N>
static ShipSet MakeShipSet(std::size_t (&lengths)[N]) {
  ShipSet set;
  set.first = &lengths[0];
  set.last = set.first + N;
  return set;
}

// Return a string representation of a ship set.
static QString MakeSetString(ShipSet set) {
  QString ret;
  if (set.first == set.last) return ret;
  ret.append(QString::number(*set.first));
  for (++set.first; set.first != set.last; ++set.first) {
    ret.append(",");
    ret.append(QString::number(*set.first));
  }
  return ret;
}

NewGameSelector::NewGameSelector(QWidget* parent)
    : QDialog(parent),
      size_radio1_(new QRadioButton(MakeSizeString(size1))),
      size_radio2_(new QRadioButton(MakeSizeString(size2))),
      size_radio3_(new QRadioButton(MakeSizeString(size3))),
      size_radio4_(new QRadioButton(MakeSizeString(size4))),
      set_radio1_(new QRadioButton(MakeSetString(MakeShipSet(set1)))),
      set_radio2_(new QRadioButton(MakeSetString(MakeShipSet(set2)))),
      set_radio3_(new QRadioButton(MakeSetString(MakeShipSet(set3)))),
      set_radio4_(new QRadioButton(MakeSetString(MakeShipSet(set4)))),
      buttons_(new QDialogButtonBox(QDialogButtonBox::Ok |
                                    QDialogButtonBox::Cancel)) {
  size_radio2_->setChecked(true);
  set_radio2_->setChecked(true);

  QHBoxLayout* size_layout = new QHBoxLayout;
  size_layout->addWidget(size_radio1_);
  size_layout->addWidget(size_radio2_);
  size_layout->addWidget(size_radio3_);
  size_layout->addWidget(size_radio4_);
  QGroupBox* size_box = new QGroupBox("Arena size");
  size_box->setLayout(size_layout);

  QHBoxLayout* set_layout = new QHBoxLayout;
  set_layout->addWidget(set_radio1_);
  set_layout->addWidget(set_radio2_);
  set_layout->addWidget(set_radio3_);
  set_layout->addWidget(set_radio4_);
  QGroupBox* set_box = new QGroupBox("Ship set");
  set_box->setLayout(set_layout);

  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->addWidget(size_box);
  vbox->addWidget(set_box);
  vbox->addWidget(buttons_);
  setLayout(vbox);
}

// Button accessor, neccesary to connect events.
QDialogButtonBox* NewGameSelector::buttons() { return buttons_; }

// Return the selected map size.
MapSize NewGameSelector::GetMapSize() {
  if (size_radio1_->isChecked()) return size1;
  if (size_radio2_->isChecked()) return size2;
  if (size_radio3_->isChecked()) return size3;
  return size4;
}

// Return the selected ship set.
ShipSet NewGameSelector::GetShipSet() {
  if (set_radio1_->isChecked()) return MakeShipSet(set1);
  if (set_radio2_->isChecked()) return MakeShipSet(set2);
  if (set_radio3_->isChecked()) return MakeShipSet(set3);
  return MakeShipSet(set4);
}

}  // namespace battleship
