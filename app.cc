#include <iostream>

#include <QApplication>

#include "display.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  Display display;
  display.queryNextFrame();
  return app.exec();
}