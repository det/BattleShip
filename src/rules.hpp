#ifndef BATTLESHIP_RULES_H
#define BATTLESHIP_RULES_H

#include <QDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextEdit>

namespace battleship {

// Modal dialog for displaying the game rules.
class Rules : public QDialog {
  Q_OBJECT
 public:
  Rules(QWidget* parent);
};

}  // namespace battleship

#endif  //#ifndef BATTLESHIP_RULES_H
