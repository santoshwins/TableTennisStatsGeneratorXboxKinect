#include <QtGui/QApplication>
#include "MainWindow.h"
/* this code runs the basic main window and is created by the Qt Creator app */
int main(int argc, char *argv[])
{
  // make an instance of the QApplication
  QApplication a(argc, argv);

  // Create a new MainWindow
  // for our application
  MainWindow w;
  // show our statistics application
  w.show();

  // return control over to Qt framework
  return a.exec();
}
