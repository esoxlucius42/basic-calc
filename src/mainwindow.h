#pragma once

#include <QString>
#include <QWidget>

#include <vector>

#include "calculator_engine.h"

class QLineEdit;
class QPushButton;

class MainWindow : public QWidget {
  Q_OBJECT

 public:
  struct ButtonSpec {
    Button id;
    const char* label;
    int row;
    int column;
    int row_span = 1;
    int column_span = 1;
    const char* style_class;
    std::vector<QString> shortcuts;
  };

  explicit MainWindow(QWidget* parent = nullptr);

 private:
  void build_ui();
  void connect_shortcuts();
  void press(Button id);
  void refresh_display();
  QPushButton* create_button(const ButtonSpec& spec);

  CalculatorEngine engine_;
  QLineEdit* display_ = nullptr;
};
