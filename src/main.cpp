#include <QApplication>
#include <QGuiApplication>
#include <QIcon>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("basic-calc");
  QApplication::setApplicationDisplayName("Basic Calc");
  QGuiApplication::setDesktopFileName("basic-calc");

  const QIcon app_icon(":/icons/sqrt-icon.png");
  app.setWindowIcon(app_icon);

  MainWindow window;
  window.setWindowIcon(app_icon);
  window.show();

  return app.exec();
}
