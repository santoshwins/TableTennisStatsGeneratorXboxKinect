#ifndef EXTENDEDQMDISUBWINDOW_H
#define EXTENDEDQMDISUBWINDOW_H

#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QMainWindow>

class MainWindow;
// we are extending from qmdiwindow as we need to detect
// which mdi window is closed by the user
// in order to try and zero out the corresponding
// widget contained in it
// without extending we CANNOT detect on closed
// event on a QMdiSubWindow
class ExtendedQMdiSubWindow : public QMdiSubWindow
{
    Q_OBJECT

public:
    ExtendedQMdiSubWindow(MainWindow *_parent = 0);


protected:
    void closeEvent( QCloseEvent * closeEvent );


private:
    MainWindow *m_ptrToMainWindow;

};

#endif // EXTENDEDQMDISUBWINDOW_H
