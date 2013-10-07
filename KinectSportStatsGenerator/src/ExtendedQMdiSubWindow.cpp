#include "ExtendedQMdiSubWindow.h"
#include "MainWindow.h"

#include <iostream>

const std::string statsWindowName = "Statsvz";
const std::string trackingWindowName = "Foregroundlive";
const std::string rgbWindowName = "Rgbvz";
const std::string depthWindowName = "Depthlive";
ExtendedQMdiSubWindow::ExtendedQMdiSubWindow(MainWindow *_parent)
{
    m_ptrToMainWindow = _parent;
}

void ExtendedQMdiSubWindow::closeEvent(QCloseEvent *closeEvent)
{
    std::cout<<"closing..";
    std::cout<<this->accessibleName().toStdString();

    if(this->accessibleName().toStdString() == statsWindowName)
    {
        m_ptrToMainWindow->deleteCurrentStatsvz();
        std::cout<<"statsclosed";
    }

    if(this->accessibleName().toStdString() == trackingWindowName)
    {
        m_ptrToMainWindow->deleteCurrentTrackingvz();
        std::cout<<"trakingvzclosed";
    }

    if(this->accessibleName().toStdString() == rgbWindowName)
    {
        m_ptrToMainWindow->deleteCurrentRGBvz();
        std::cout<<"rgbclosed";
    }

    if(this->accessibleName().toStdString() == depthWindowName)
    {
        m_ptrToMainWindow->deleteCurrentDepthvz();
        std::cout<<"depthclosed";
    }

    //emit closed( this->objectName() );
    closeEvent->accept();
}
