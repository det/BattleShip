#include "game.hpp"

#include <QDialog>
#include <QMessageBox>
#include <QTextEdit>

namespace battleship {

static char const* const kRules =
    "Rules:\n\n"

    "At the beginning of the game each player will place 5 ships in their "
    "opponents arena. The lengths of ships to be placed are 2, 3, 3, 4, and 5. "
    "A ship is placed by holding a mouse button and dragging a vertical or "
    "horizontal line along the cells a player wishes to place a ship onto. For "
    "example, dragging a line across 4 cells will place a ship of length 4. "
    "Ships may not overlap. When Player1 has finished placing ships on "
    "Player2's arena, Player2 will place their ships on Player1's arena.\n\n"

    "When the placing round is over, players will alternate in making attacks. "
    "After each attack, markers will be placed indicating hits and misses. "
    "Sunk ships will also be revealed. When a player has sunk all of the "
    "opponents ships, that player has won the game.";

// Size constants.
static std::size_t kWidth = 10;
static std::size_t kHeight = 10;

enum CheckPlacementType { kFreeIndex, kAlreadyPlaced, kInvalidLength };

struct CheckPlacementResult {
  CheckPlacementType type;
  std::size_t index;
};

// Add the unplaced ships to a string.
static void AppendRemainingShips(QString& string, std::bitset<5> set) {
  if (!set[0]) string.append(" 2");
  if (!set[1]) string.append(" 3");
  if (!set[2]) string.append(" 3");
  if (!set[3]) string.append(" 4");
  if (!set[4]) string.append(" 5");
}

// Check to see if the specified length has been placed yet.
static CheckPlacementResult CheckPlacement(std::bitset<5> set,
                                           std::size_t length) {
  CheckPlacementResult res;
  res.type = kFreeIndex;
  switch (length) {
    case 2:
      if (set[0]) res.type = kAlreadyPlaced;
      res.index = 0;
      break;

    case 3:
      if (set[1]) {
        if (set[2])
          res.type = kAlreadyPlaced;
        else
          res.index = 2;
      } else
        res.index = 1;
      break;

    case 4:
      if (set[3]) res.type = kAlreadyPlaced;
      res.index = 3;
      break;

    case 5:
      if (set[4]) res.type = kAlreadyPlaced;
      res.index = 4;
      break;

    default:
      res.type = kInvalidLength;
  }
  return res;
}

// Try to place a ship on the specified group.
bool Game::PlaceShip(Group& group, Ship const& ship) {
  CheckPlacementResult res1 = CheckPlacement(group.ships_placed, ship.length);
  if (res1.type == kAlreadyPlaced) {
    QString string = "This ship has already been placed. Remaining lengths:";
    AppendRemainingShips(string, group.ships_placed);
    status_bar_->showMessage(string);
    return false;
  }
  if (res1.type == kInvalidLength) {
    QString string = "This is not a valid length. Remaining lengths:";
    AppendRemainingShips(string, group.ships_placed);
    status_bar_->showMessage(string);
    return false;
  }

  Board::PlaceResult res2 = group.board.Place(ship);
  switch (res2.type) {
    case Board::kPlaced:
      group.arena->AddReveal(*res2.ship);
      ++group.ships_left;
      status_bar_->showMessage("Ship has been placed.");
      group.ships_placed[res1.index] = true;
      return true;
    case Board::kOverlap:
      status_bar_->showMessage("Ships may not overlap.");
      return false;
  }

  return false;
}

// Try to attack a cell on a specified group.
bool Game::Attack(Group& group, std::size_t x, std::size_t y) {
  Board::AttackResult res = group.board.Attack(x, y);
  switch (res.type) {
    case Board::kSunk:
      status_bar_->showMessage("You sunk a ship!");
      group.arena->AddSunk(*res.ship);
      group.arena->AddHit(x, y);
      --group.ships_left;
      return true;

    case Board::kHit:
      status_bar_->showMessage("Hit!");
      group.arena->AddHit(x, y);
      return true;

    case Board::kMiss:
      status_bar_->showMessage("Miss!");
      group.arena->AddMiss(x, y);
      return true;

    case Board::kRetry:
      status_bar_->showMessage("You've already attacked here.");
      return false;
  }

  return false;
}

// Allow placement of ships on arena1.
void Game::BeginPlacing1() {
  group1_.arena->SetPlacing();
  group2_.arena->SetDisplaying();
  group1_.arena->setStatusTip(
      "Player2: Click-drag on the left arena to place your ships.");
  group2_.arena->setStatusTip("");
}

// Allow placement of ships on arena2.
void Game::BeginPlacing2() {
  group2_.arena->SetPlacing();
  group1_.arena->SetDisplaying();
  group1_.arena->setStatusTip("");
  group2_.arena->setStatusTip(
      "Player1: Click-drag on the right arena to place your ships.");
}

// Allow attacks on arena1.
void Game::BeginAttacking1() {
  group1_.arena->SetAttacking();
  group2_.arena->SetDisplaying();
  group1_.arena->setStatusTip("Player1: Click a square to make an attack.");
  group2_.arena->setStatusTip("");
}

// Allow attacks on arena2.
void Game::BeginAttacking2() {
  group2_.arena->SetAttacking();
  group1_.arena->SetDisplaying();
  group1_.arena->setStatusTip("");
  group2_.arena->setStatusTip("Player2: Click a square to make an attack.");
}

// Handle attacks on arena1.
void Game::HandleAttacked1(std::size_t x, std::size_t y) {
  if (!Attack(group1_, x, y)) return;

  if (group1_.ships_left == 0) {
    QMessageBox::information(this, "BattleShip", "Player1 wins!");
    group1_.arena->setStatusTip("");
    group1_.arena->SetRevealing();
    group2_.arena->SetRevealing();
    return;
  }

  BeginAttacking2();
}

// Handle attacks on arena2.
void Game::HandleAttacked2(std::size_t x, std::size_t y) {
  if (!Attack(group2_, x, y)) return;

  if (group2_.ships_left == 0) {
    QMessageBox::information(this, "BattleShip", "Player2 wins!");
    group2_.arena->setStatusTip("");
    group1_.arena->SetRevealing();
    group2_.arena->SetRevealing();
    return;
  }

  BeginAttacking1();
}

// Handle a placed ship on arena1.
void Game::HandleShipPlaced1(Ship const& ship) {
  if (!PlaceShip(group1_, ship)) return;

  if (group1_.ships_placed.all()) {
    QMessageBox::information(this, "BattleShip",
                             "All ships have been deployed for Player2.");
    BeginAttacking1();
  }
}

// Handle a placed ship on arena2.
void Game::HandleShipPlaced2(Ship const& ship) {
  if (!PlaceShip(group2_, ship)) return;

  if (group2_.ships_placed.all()) {
    QMessageBox::information(this, "BattleShip",
                             "All ships have been deployed for Player1.");
    BeginPlacing1();
  }
}

// Start a new game as triggerd by the menu.
void Game::HandleNewGame(bool) {
  group1_.arena->Init(kWidth, kHeight);
  group2_.arena->Init(kWidth, kHeight);
  group1_.board.Init(kWidth, kHeight);
  group2_.board.Init(kWidth, kHeight);
  group1_.ships_left = 0;
  group2_.ships_left = 0;
  group1_.ships_placed.reset();
  group2_.ships_placed.reset();
  BeginPlacing2();
}

// Display the rules as triggerd by the menu.
void Game::HandleHowToPlay(bool) {
  QTextEdit* text_edit = new QTextEdit;
  text_edit->setText(kRules);
  text_edit->setReadOnly(true);
  QHBoxLayout* layout = new QHBoxLayout;
  layout->addWidget(text_edit);
  QDialog dialog(this);
  dialog.setModal(true);
  dialog.setLayout(layout);
  dialog.resize(640, 480);
  dialog.exec();
}

// Construct an arena and wait for a new game.
Game::Game()
    : menu_bar_(new QMenuBar),
      layout_(new QHBoxLayout),
      status_bar_(new QStatusBar) {
  group1_.arena = new Arena(kWidth, kHeight);
  group2_.arena = new Arena(kWidth, kHeight);

  QAction* new_game = new QAction("New game", this);
  QMenu* file_menu = menu_bar_->addMenu("&File");
  file_menu->addAction(new_game);

  QAction* how_to_play = new QAction("How to play", this);
  QMenu* help_menu = menu_bar_->addMenu("&Help");
  help_menu->addAction(how_to_play);

  setMenuBar(menu_bar_);
  layout_->addWidget(group1_.arena);
  layout_->addWidget(group2_.arena);
  QWidget* widget = new QWidget;
  widget->setLayout(layout_);
  setCentralWidget(widget);
  setStatusBar(status_bar_);

  // Connect callbacks.
  connect(new_game, &QAction::triggered, this, &Game::HandleNewGame);
  connect(how_to_play, &QAction::triggered, this, &Game::HandleHowToPlay);
  connect(group1_.arena, &Arena::Attacked, this, &Game::HandleAttacked1);
  connect(group1_.arena, &Arena::ShipPlaced, this, &Game::HandleShipPlaced1);
  connect(group2_.arena, &Arena::Attacked, this, &Game::HandleAttacked2);
  connect(group2_.arena, &Arena::ShipPlaced, this, &Game::HandleShipPlaced2);
}
}  // namespace battleship
