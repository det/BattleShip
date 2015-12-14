
#include "rules.hpp"

namespace battleship {

static char const* const kRules =
    "Rules:\n\n"

    "At the beginning of the game each player will place all ships in their "
    "opponent's arena. The lengths of ships to be placed are determined when "
    "creating a new game. A ship is placed by holding a mouse button and "
    "dragging a vertical or horizontal line along the cells a player wishes to "
    "place a ship onto. For example, dragging a line across 4 cells will place "
    "a ship of length 4. Ships may not overlap. When Player1 has finished "
    "placing ships on Player2's arena, Player2 will place their ships on "
    "Player1's arena.\n\n"

    "When the placing round is over, players will alternate in making attacks. "
    "After each attack, markers will be placed indicating hits and misses. "
    "Sunk ships will also be revealed. When a player has sunk all of the "
    "opponents ships, that player has won the game.";

Rules::Rules(QWidget* parent) : QDialog(parent) {
  QTextEdit* text_edit = new QTextEdit;
  text_edit->setText(kRules);
  text_edit->setReadOnly(true);
  QHBoxLayout* layout = new QHBoxLayout;
  layout->addWidget(text_edit);
  setLayout(layout);
  resize(640, 480);
}

}  // namespace battleship
