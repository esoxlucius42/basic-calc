#include "mainwindow.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShortcut>
#include <QSizePolicy>
#include <QStringList>
#include <QVBoxLayout>

#include <vector>

namespace {

using Spec = MainWindow::ButtonSpec;

std::vector<Spec> button_specs() {
  return {
      {Button::ToggleSign, "+/-", 0, 0, 1, 1, "accent", {"N"}},
      {Button::SquareRoot, "\xE2\x88\x9A", 0, 1, 1, 1, "accent", {"S"}},
      {Button::Divide, "\xC3\xB7", 0, 2, 1, 1, "operator", {"/"}},
      {Button::Add, "+", 0, 3, 1, 1, "operator", {"+"}},

      {Button::Digit7, "7", 1, 0, 1, 1, "digit", {"7"}},
      {Button::Digit8, "8", 1, 1, 1, 1, "digit", {"8"}},
      {Button::Digit9, "9", 1, 2, 1, 1, "digit", {"9"}},
      {Button::Multiply, "\xC3\x97", 1, 3, 1, 1, "operator", {"*", "X"}},

      {Button::Digit4, "4", 2, 0, 1, 1, "digit", {"4"}},
      {Button::Digit5, "5", 2, 1, 1, 1, "digit", {"5"}},
      {Button::Digit6, "6", 2, 2, 1, 1, "digit", {"6"}},
      {Button::Subtract, "-", 2, 3, 1, 1, "operator", {"-"}},

      {Button::Digit1, "1", 3, 0, 1, 1, "digit", {"1"}},
      {Button::Digit2, "2", 3, 1, 1, 1, "digit", {"2"}},
      {Button::Digit3, "3", 3, 2, 1, 1, "digit", {"3"}},
      {Button::Equals, "=", 3, 3, 2, 1, "equals", {"=", "Return", "Enter"}},

      {Button::Digit0, "0", 4, 0, 1, 2, "digit", {"0"}},
      {Button::Decimal, ".", 4, 2, 1, 1, "digit", {".", ","}},
  };
}

QString shortcut_text(const Spec& spec) {
  if (spec.shortcuts.empty()) {
    return {};
  }

  QStringList parts;
  for (const QString& shortcut : spec.shortcuts) {
    parts << shortcut;
  }
  return parts.join(", ");
}

}  // namespace

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
  build_ui();
  refresh_display();
  setWindowTitle("calc");
  setFixedSize(392, 660);
}

void MainWindow::build_ui() {
  setObjectName("window");
  setStyleSheet(R"(
    QWidget#window {
      background: #f6f3eb;
    }
    QFrame#calculatorShell {
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                  stop: 0 #f3f4f0, stop: 1 #cfd3d7);
      border-radius: 30px;
      border: 1px solid #b2b5ba;
    }
    QFrame#topPanel {
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                  stop: 0 #494f57, stop: 1 #24282f);
      border-radius: 20px;
      border: 1px solid #171a1f;
    }
    QLabel[role="solar-cell"], QLabel[role="solar-cell-left"], QLabel[role="solar-cell-right"] {
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                  stop: 0 #f6eee4, stop: 1 #e3d6c7);
      border: 1px solid #c4b4a0;
      min-height: 20px;
      min-width: 24px;
    }
    QLabel[role="solar-cell-left"] {
      border-top-left-radius: 3px;
      border-bottom-left-radius: 3px;
    }
    QLabel[role="solar-cell-right"] {
      border-top-right-radius: 3px;
      border-bottom-right-radius: 3px;
    }
    QFrame#displayChin {
      background: #6d746e;
      border-radius: 12px;
      border: 1px solid #4d534f;
      min-height: 20px;
    }
    QFrame#displayBezel {
      background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1,
                                  stop: 0 #d3d8db, stop: 1 #a7afb6);
      border-radius: 16px;
      border: 1px solid #8c949c;
    }
    QLineEdit#display {
      background: #eef3dd;
      color: #1f2620;
      border: 2px solid #7a837d;
      border-radius: 8px;
      font: 700 31px "DejaVu Sans";
      padding: 12px 14px;
      selection-background-color: #b8c69a;
    }
    QPushButton {
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                  stop: 0 #4e5661, stop: 1 #2c3139);
      color: #f5f7f8;
      border-radius: 12px;
      min-height: 68px;
      font: 700 22px "DejaVu Sans";
      border: 1px solid #747a82;
      padding-bottom: 2px;
    }
    QPushButton[class="accent"] {
      font-size: 20px;
    }
    QPushButton[class="operator"], QPushButton[class="equals"] {
      font-size: 30px;
    }
    QPushButton:pressed {
      padding-top: 2px;
      padding-bottom: 0;
    }
  )");

  auto* root_layout = new QVBoxLayout(this);
  root_layout->setContentsMargins(0, 0, 0, 0);

  auto* calculator_shell = new QFrame(this);
  calculator_shell->setObjectName("calculatorShell");
  root_layout->addWidget(calculator_shell);

  auto* shell_layout = new QVBoxLayout(calculator_shell);
  shell_layout->setContentsMargins(18, 16, 18, 18);
  shell_layout->setSpacing(16);

  auto* top_panel = new QFrame(calculator_shell);
  top_panel->setObjectName("topPanel");
  shell_layout->addWidget(top_panel);

  auto* top_panel_layout = new QVBoxLayout(top_panel);
  top_panel_layout->setContentsMargins(16, 12, 16, 16);
  top_panel_layout->setSpacing(12);

  auto* solar_layout = new QHBoxLayout();
  solar_layout->addStretch();
  for (int i = 0; i < 4; ++i) {
    auto* cell = new QLabel(top_panel);
    cell->setProperty("role", i == 0 ? "solar-cell-left" : (i == 3 ? "solar-cell-right" : "solar-cell"));
    cell->setMinimumSize(24, 20);
    solar_layout->addWidget(cell);
  }
  top_panel_layout->addLayout(solar_layout);

  auto* bezel = new QFrame(top_panel);
  bezel->setObjectName("displayBezel");
  top_panel_layout->addWidget(bezel);

  auto* bezel_layout = new QHBoxLayout(bezel);
  bezel_layout->setContentsMargins(14, 14, 14, 14);

  display_ = new QLineEdit(bezel);
  display_->setObjectName("display");
  display_->setReadOnly(true);
  display_->setAlignment(Qt::AlignRight);
  display_->setCursor(Qt::ArrowCursor);
  bezel_layout->addWidget(display_);

  auto* display_chin = new QFrame(top_panel);
  display_chin->setObjectName("displayChin");
  display_chin->setFixedWidth(150);
  top_panel_layout->addWidget(display_chin, 0, Qt::AlignHCenter);

  auto* keypad_layout = new QGridLayout();
  keypad_layout->setHorizontalSpacing(10);
  keypad_layout->setVerticalSpacing(10);
  shell_layout->addLayout(keypad_layout, 1);

  for (const auto& spec : button_specs()) {
    keypad_layout->addWidget(create_button(spec), spec.row, spec.column, spec.row_span, spec.column_span);
  }

  connect_shortcuts();
}

QPushButton* MainWindow::create_button(const ButtonSpec& spec) {
  auto* button = new QPushButton(QString::fromUtf8(spec.label), this);
  button->setProperty("class", QString::fromUtf8(spec.style_class));
  button->setFocusPolicy(Qt::NoFocus);
  button->setCursor(Qt::PointingHandCursor);
  button->setAutoDefault(false);
  button->setDefault(false);
  if (spec.id == Button::Equals) {
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  }

  const QString shortcuts = shortcut_text(spec);
  if (!shortcuts.isEmpty()) {
    button->setToolTip(QString("%1 [%2]").arg(spec.label, shortcuts));
  }

  connect(button, &QPushButton::clicked, this, [this, id = spec.id]() { press(id); });
  return button;
}

void MainWindow::connect_shortcuts() {
  for (const auto& spec : button_specs()) {
    for (const QString& shortcut_text_value : spec.shortcuts) {
      auto* shortcut = new QShortcut(QKeySequence(shortcut_text_value), this);
      shortcut->setContext(Qt::WindowShortcut);
      connect(shortcut, &QShortcut::activated, this, [this, id = spec.id]() { press(id); });
    }
  }

  auto* reset_shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
  reset_shortcut->setContext(Qt::WindowShortcut);
  connect(reset_shortcut, &QShortcut::activated, this, [this]() {
    engine_.reset();
    refresh_display();
  });
}

void MainWindow::press(Button id) {
  engine_.press(id);
  refresh_display();
}

void MainWindow::refresh_display() {
  display_->setText(QString::fromStdString(engine_.display_text()));
}
