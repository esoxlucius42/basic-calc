#include <QApplication>
#include <QIcon>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  const QIcon app_icon(":/icons/sqrt-icon.png");
  app.setWindowIcon(app_icon);

  MainWindow window;
  window.setWindowIcon(app_icon);
  window.show();

  return app.exec();
}
