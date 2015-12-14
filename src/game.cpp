#include "game.hpp"

#include <QMessageBox>

namespace battleship {

// Add the unplaced ships to a string.
static void AppendRemainingShips(QString& string,
                                 std::vector<std::size_t> const& ship_set) {
  if (ship_set.empty()) return;
  string.append(QString::number(ship_set[0]));
  for (std::size_t i = 1, e = ship_set.size(); i != e; ++i) {
    string.append(", ");
    string.append(QString::number(ship_set[i]));
  }
}

// Try to place a ship on the specified group.
bool Game::PlaceShip(Group& group, Ship const& ship) {
  std::vector<std::size_t>::iterator it =
      std::find(group.ship_set.begin(), group.ship_set.end(), ship.length);
  if (it == group.ship_set.end()) {
    QString string = "This is not a valid length. Remaining lengths: ";
    AppendRemainingShips(string, group.ship_set);
    status_bar_->showMessage(string);
    return false;
  }

  Board::PlaceResult res2 = group.board.Place(ship);
  switch (res2.type) {
    case Board::kPlaced:
      group.arena->AddReveal(*res2.ship);
      ++group.ships_left;
      status_bar_->showMessage("Ship has been placed.");
      group.ship_set.erase(it);
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
  group1_.arena->setStatusTip("Player1: Click a cell to make an attack.");
  group2_.arena->setStatusTip("");
}

// Allow attacks on arena2.
void Game::BeginAttacking2() {
  group2_.arena->SetAttacking();
  group1_.arena->SetDisplaying();
  group1_.arena->setStatusTip("");
  group2_.arena->setStatusTip("Player2: Click a cell to make an attack.");
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

  if (group1_.ship_set.empty()) {
    QMessageBox::information(this, "BattleShip",
                             "All ships have been deployed for Player2.");
    BeginAttacking1();
  }
}

// Handle a placed ship on arena2.
void Game::HandleShipPlaced2(Ship const& ship) {
  if (!PlaceShip(group2_, ship)) return;

  if (group2_.ship_set.empty()) {
    QMessageBox::information(this, "BattleShip",
                             "All ships have been deployed for Player1.");
    BeginPlacing1();
  }
}

// Dont do anything.
void Game::HandleCancelNewGame() { game_selection_.close(); }

// Create a new game from the selected size.
void Game::HandleCreateNewGame() {
  MapSize size = game_selection_.GetMapSize();
  ShipSet set = game_selection_.GetShipSet();
  group1_.arena->Init(size.x, size.y);
  group2_.arena->Init(size.x, size.y);
  group1_.board.Init(size.x, size.y);
  group2_.board.Init(size.x, size.y);
  group1_.ships_left = 0;
  group2_.ships_left = 0;
  group1_.ship_set.assign(set.first, set.last);
  group2_.ship_set.assign(set.first, set.last);
  game_selection_.close();
  BeginPlacing2();
}

// Open the new game dialog.
void Game::HandleNewGame(bool) { game_selection_.exec(); }

// Exit the game.
void Game::HandleExit(bool) { close(); }

// Open the new rules dialog.
void Game::HandleHowToPlay(bool) { rules_.exec(); }

// Resize the window when our layout changes.
bool Game::event(QEvent* event) {
  if (event->type() == QEvent::LayoutRequest) setFixedSize(sizeHint());
  return QMainWindow::event(event);
}

// Construct a Game and wait for a new game.
Game::Game(std::size_t width, std::size_t height)
    : game_selection_(this),
      rules_(this),
      menu_bar_(new QMenuBar),
      layout_(new QHBoxLayout),
      status_bar_(new QStatusBar) {
  group1_.arena = new Arena(width, height);
  group2_.arena = new Arena(width, height);

  QAction* new_game = new QAction("New game", this);
  QAction* exit = new QAction("Exit", this);
  QMenu* file_menu = menu_bar_->addMenu("&File");
  file_menu->addAction(new_game);
  file_menu->addAction(exit);

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
  connect(exit, &QAction::triggered, this, &Game::HandleExit);
  connect(how_to_play, &QAction::triggered, this, &Game::HandleHowToPlay);
  connect(group1_.arena, &Arena::Attacked, this, &Game::HandleAttacked1);
  connect(group1_.arena, &Arena::ShipPlaced, this, &Game::HandleShipPlaced1);
  connect(group2_.arena, &Arena::Attacked, this, &Game::HandleAttacked2);
  connect(group2_.arena, &Arena::ShipPlaced, this, &Game::HandleShipPlaced2);
  connect(game_selection_.buttons(), &QDialogButtonBox::accepted, this,
          &Game::HandleCreateNewGame);
  connect(game_selection_.buttons(), &QDialogButtonBox::rejected, this,
          &Game::HandleCancelNewGame);
}

}  // namespace battleship
