#include "RGBboundsVisualization.h"
#include "BallTrackingVisualization.h"
#include "BallTrackingUtility.h"
#include "DepthDebugVisualization.h"
#include "StatsVisualization.h"
#include "PlayerData.h"
#include "BallPointsProcessing.h"

#include "MainWindow.h"
#include <QToolBar>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>
#include <QApplication>
#include <QLabel>
#include <QRadioButton>
#include <QMdiSubWindow>
#include <QDebug>
#include <ngl/ShaderLib.h>

MainWindow::MainWindow(
                       QWidget *_parent
                      ):
                        QMainWindow(_parent)
{
    this->resize(QSize(1920,1200));

    m_liveWindowBuilt = false;

    m_toolBarStart = new QToolBar(this);
    m_toolBarStart->setObjectName("HomePage");
    QPushButton *live = new QPushButton("GO LIVE");
    QObject::connect(live,SIGNAL(clicked()),this,SLOT(buildLiveWindowSetUp()));

    QPushButton *importAnalysis = new QPushButton("IMPORT STATISTICS");
    QObject::connect(importAnalysis,SIGNAL(clicked()),this,SLOT(buildImportWindowSetUp()));

    QPushButton *setUp = new QPushButton("SETUP TRACKING");
    QObject::connect(setUp,SIGNAL(clicked()),this,SLOT(buildTrackingVzWindowSetUp()));


    this->addToolBar(m_toolBarStart);


    QWidget *w = new QWidget(m_toolBarStart); // "this" is a pointer to a QToolBar
    QVBoxLayout *vbox = new QVBoxLayout(w);
    vbox->addStretch();
    vbox->addWidget(live);
    vbox->addWidget(importAnalysis);
    vbox->addWidget(setUp);
    w->setLayout(vbox);
    m_toolBarStart->addWidget(w);

    m_toolbarOrig = new QToolBar(this);
    m_toolbarOrig->setObjectName("KinectToolbar");

    m_toolbarImport = new QToolBar(this);
    m_toolbarImport->setObjectName("ImportToolBar");

    m_toolbarPlayer = new QToolBar(this);
    m_toolbarPlayer->setObjectName("PlayerToolBar");

    // this button should only be der in the import window
    QPushButton *readFile = new QPushButton("Read File");
    QObject::connect(readFile,SIGNAL(clicked()),this,SLOT(readFile()));
    m_toolbarImport->addWidget(readFile);

    m_toolbar = new QToolBar(this);
    m_toolbar->setObjectName("KinectDebugToolbarLive");


    m_lowerDepthLabel= new QLabel(m_toolbar);
    m_lowerDepthLabel->setText("Lower Depth Volume Threshold");
    m_toolbar->addWidget(m_lowerDepthLabel);

    m_depthVolumeLowerThresh = new QDoubleSpinBox(m_toolbar);
    m_depthVolumeLowerThresh->setMinimum(0.01);
    m_depthVolumeLowerThresh->setMaximum(6.0);
    m_depthVolumeLowerThresh->setValue(0.02);
    m_depthVolumeLowerThresh->setSingleStep(0.1);
    m_toolbar->addWidget(m_depthVolumeLowerThresh);

    m_higherDepthLabel= new QLabel(m_toolbar);
    m_higherDepthLabel->setText("Higher Depth Volume Threshold");
    m_toolbar->addWidget(m_higherDepthLabel);

    m_depthVolumeHigherThresh = new QDoubleSpinBox(m_toolbar);
    m_depthVolumeHigherThresh->setMinimum(0.20);
    m_depthVolumeHigherThresh->setMaximum(6.0);
    m_depthVolumeHigherThresh->setValue(1.02);
    m_depthVolumeHigherThresh->setSingleStep(0.1);
    m_toolbar->addWidget(m_depthVolumeHigherThresh);


    m_lowerBinLabel= new QLabel(m_toolbar);
    m_lowerBinLabel->setText("Binary Lower Threshold");
    m_toolbar->addWidget(m_lowerBinLabel);

    m_binLowerSpinBox = new QDoubleSpinBox(m_toolbar);
    m_binLowerSpinBox->setMaximum(255);
    m_binLowerSpinBox->setValue(100.0);
    m_binLowerSpinBox->setSingleStep(5.0);
    m_toolbar->addWidget(m_binLowerSpinBox);

    // higher threshold not needed..its max value is to be FIXED as 255 which is done in the BallTrackingVisualization constructor

    m_cannyLowerLabel= new QLabel(m_toolbar);
    m_cannyLowerLabel->setText("Canny Lower Threshold");
    m_toolbar->addWidget(m_cannyLowerLabel);

    m_cannyLowerSpinBox = new QDoubleSpinBox(m_toolbar);
    //slider->setMinimum(100);
    //slider->setMaximum(255);
    m_cannyLowerSpinBox->setMaximum(255);
    m_cannyLowerSpinBox->setValue(100.0);
    m_cannyLowerSpinBox->setSingleStep(5.0);
    //QObject::connect(m_cannyLowerSpinBox,SIGNAL(valueChanged(double)),m_trackingInBackground,SLOT(setCannyLowerThresh(double)));
    m_toolbar->addWidget(m_cannyLowerSpinBox);

    m_cannyHigherLabel = new QLabel(m_toolbar);
    m_cannyHigherLabel->setText("Canny Higher Threshold");
    m_toolbar->addWidget(m_cannyHigherLabel);

    m_cannyHigherSpinBox = new QDoubleSpinBox(m_toolbar);
    //slider->setMinimum(100);
    //slider->setMaximum(255);
    m_cannyHigherSpinBox->setMaximum(500);
    m_cannyHigherSpinBox->setValue(300.0);
    m_cannyHigherSpinBox->setSingleStep(5.0);
    //QObject::connect(m_cannyHigherSpinBox,SIGNAL(valueChanged(double)),m_trackingInBackground,SLOT(setCannyHigherThresh(double)));
    m_toolbar->addWidget(m_cannyHigherSpinBox);

    m_contourAreaLabel = new QLabel(m_toolbar);
    m_contourAreaLabel->setText("Contour Area Threshold");
    m_toolbar->addWidget(m_contourAreaLabel);

    m_contourAreaThresh = new QDoubleSpinBox(m_toolbar);
    //slider->setMinimum(100);
    //slider->setMaximum(255);
    m_contourAreaThresh->setMinimum(5.0);
    m_contourAreaThresh->setMaximum(1500.0);
    m_contourAreaThresh->setValue(120.0);
    m_contourAreaThresh->setSingleStep(5.0);
    //QObject::connect(m_contourAreaThresh,SIGNAL(valueChanged(double)),m_trackingInBackground,SLOT(setContourAreaThresh(double)));
    m_toolbar->addWidget(m_contourAreaThresh);


    m_impPointDepthMinLabel = new QLabel(m_toolbar);
    m_impPointDepthMinLabel->setText("Impact point depth Min thresh");
    m_toolbar->addWidget(m_impPointDepthMinLabel);

    m_impactPtMinDepth = new QDoubleSpinBox(m_toolbar);
    //slider->setMinimum(100);
    //slider->setMaximum(255);
    m_impactPtMinDepth->setMinimum(0.02);
    m_impactPtMinDepth->setMaximum(5.0);
    m_impactPtMinDepth->setValue(0.60);
    m_impactPtMinDepth->setSingleStep(0.01);
    //QObject::connect(m_impactPtMinDepth,SIGNAL(valueChanged(double)),m_processingUtility,SLOT(setBallImpactDepthMinThreshold(double)));
    m_toolbar->addWidget(m_impactPtMinDepth);

    m_impPointDepthMaxLabel = new QLabel(m_toolbar);
    m_impPointDepthMaxLabel->setText("Impact point depth Max thresh");
    m_toolbar->addWidget(m_impPointDepthMaxLabel);

    m_impactPtMaxDepth = new QDoubleSpinBox(m_toolbar);
    //slider->setMinimum(100);
    //slider->setMaximum(255);
    m_impactPtMaxDepth->setMinimum(0.02);
    m_impactPtMaxDepth->setMaximum(5.0);
    m_impactPtMaxDepth->setValue(0.70);
    m_impactPtMaxDepth->setSingleStep(0.01);
    //QObject::connect(m_impactPtMaxDepth,SIGNAL(valueChanged(double)),m_processingUtility,SLOT(setBallImpactDepthMaxThreshold(double)));
    m_toolbar->addWidget(m_impactPtMaxDepth);


    m_diffAreaLabel = new QLabel(m_toolbar);
    m_diffAreaLabel->setText("Difference Area Threshold");
    m_toolbar->addWidget(m_diffAreaLabel);

    m_diffAreaThresh = new QDoubleSpinBox(m_toolbar);
    //slider->setMinimum(100);
    //slider->setMaximum(255);
    m_diffAreaThresh->setMinimum(5.0);
    m_diffAreaThresh->setMaximum(500.0);
    m_diffAreaThresh->setValue(30.0);
    m_diffAreaThresh->setSingleStep(5.0);
    //QObject::connect(m_diffAreaThresh,SIGNAL(valueChanged(double)),m_trackingInBackground,SLOT(setDifferenceInAreaThresh(double)));
    m_toolbar->addWidget(m_diffAreaThresh);

    m_ellipseLabel = new QLabel(m_toolbar);
    m_ellipseLabel->setText("EllipticalStructuring Size");
    m_toolbar->addWidget(m_ellipseLabel);

     m_ellipseSize = new QSlider(m_toolbar);
     //slider->setMinimum(100);
     //slider->setMaximum(255);
     m_ellipseSize->setMinimum(1);
     m_ellipseSize->setMaximum(20);
     m_ellipseSize->setValue(2);

     m_ellipseSize->setOrientation(Qt::Horizontal);
     //QObject::connect(m_ellipseSize,SIGNAL(sliderMoved(int)),m_trackingInBackground,SLOT(setStructuringElementSize(int)));
     m_toolbar->addWidget(m_ellipseSize);

     m_blurLabel = new QLabel(m_toolbar);
     m_blurLabel->setText("Blur Size");
     m_toolbar->addWidget(m_blurLabel);

     m_blurSize = new QSlider(m_toolbar);
     //slider->setMinimum(100);
     //slider->setMaximum(255);
     m_blurSize->setMinimum(1);
     m_blurSize->setMaximum(20);
     m_blurSize->setValue(5);

     m_blurSize->setOrientation(Qt::Horizontal);
     //QObject::connect(m_blurSize,SIGNAL(sliderMoved(int)),m_trackingInBackground,SLOT(setBlurSize(int)));
     m_toolbar->addWidget(m_blurSize);
     m_dialationLabel = new QLabel(m_toolbar);
     m_dialationLabel->setText("No of Dialations");
     m_toolbar->addWidget(m_dialationLabel);
     m_dialation = new QSlider(m_toolbar);
     m_dialation->setMinimum(1);
     m_dialation->setMaximum(10);
     m_dialation->setValue(2);
     m_dialation->setOrientation(Qt::Horizontal);
     //QObject::connect(m_dialation,SIGNAL(sliderMoved(int)),m_trackingInBackground,SLOT(setNumerOfDialtions(int)));
     m_toolbar->addWidget(m_dialation);


     m_startTracking = new QPushButton("START TRACKING");
     QObject::connect(m_startTracking,SIGNAL(clicked()),this,SLOT(setTrackingFlag()));
     m_toolbarOrig->addWidget(m_startTracking);
     m_distanceToTableLabel = new QLabel(m_toolbar);
     m_distanceToTableLabel->setText("Distance Kinect to PlayArea(in Meters)");
     m_toolbar->addWidget(m_distanceToTableLabel);
     m_kinectToPlayArea = new QDoubleSpinBox(m_toolbar);
     //slider->setMinimum(100);
     //slider->setMaximum(255);
     m_kinectToPlayArea->setMinimum(0.2);
     m_kinectToPlayArea->setMaximum(6.0);
     m_kinectToPlayArea->setValue(0.76);
     m_kinectToPlayArea->setSingleStep(0.1);
     //QObject::connect(m_kinectToPlayArea,SIGNAL(valueChanged(double)),m_vz,SLOT(setKinectToPlayAreaDistance(double)));
     m_toolbar->addWidget(m_kinectToPlayArea);
     m_diameterBallLabel = new QLabel(m_toolbar);
     m_diameterBallLabel->setText("Diameter of ball(in Meters)");
     m_toolbar->addWidget(m_diameterBallLabel);

      m_diameterOfBall = new QDoubleSpinBox(m_toolbar);
      //slider->setMinimum(100);
      //slider->setMaximum(255);
      m_diameterOfBall->setMinimum(0.1);
      m_diameterOfBall->setMaximum(0.75);
      m_diameterOfBall->setValue(0.04);
      m_diameterOfBall->setSingleStep(0.01);
      //QObject::connect(m_diameterOfBall,SIGNAL(valueChanged(double)),m_vz,SLOT(setDiameterOfBallInMeters(double)));
      m_toolbar->addWidget(m_diameterOfBall);

      // currently directly connected to ballptsprocessing slot
      // later connect it locally to check if tracking has been stopped
      // if not warn user to stop processing before processing

      // disable process stats and other drop downs by default
      // put process stats and other stats in a group box and toggle that groupbox' visibility
      processStats = new QPushButton("PROCESS STATS");
      QObject::connect(processStats,SIGNAL(clicked()),this,SLOT(processTrackingData()));
      m_toolbarOrig->addWidget(processStats);

      processStats->setDisabled(true);

      m_writeFile = new QPushButton("Write File");
      //QObject::connect(m_writeFile,SIGNAL(clicked()),m_vz,SLOT(writeFile()));
      m_toolbarOrig->addWidget(m_writeFile);


      // see IPL app and change names and design UI accordingly
      m_choosePlayerLabel = new QLabel(m_toolbarPlayer);
      m_choosePlayerLabel->setText("Choose Player");
      m_toolbarPlayer->addWidget(m_choosePlayerLabel);

      m_playerSelection = new QComboBox(m_toolbarPlayer);
      //slider->setMinimum(100);
      //slider->setMaximum(255);
      m_playerSelection->addItem("PLAYER A"); // try making this into player names..initialize it by def
                                            // to playera and playerb to use when no name supplied
      m_playerSelection->addItem("PLAYER B");
      m_playerSelection->addItem("BOTH");
      m_playerSelection->setCurrentIndex(2);
      QObject::connect(m_playerSelection,SIGNAL(currentIndexChanged(int)),this,SLOT(setWhoseStatToView(int)));
      m_toolbarPlayer->addWidget(m_playerSelection);

      m_chooseStatsLabel = new QLabel(m_toolbarPlayer);
      m_chooseStatsLabel->setText("Choose Statistics");
      m_toolbarPlayer->addWidget(m_chooseStatsLabel);

      m_statsSelection = new QComboBox(m_toolbarPlayer);
      //slider->setMinimum(100);
      //slider->setMaximum(255);
      m_statsSelection->addItem("PITCH MAP"); // try making this into player names..initialize it by def
                                            // to playera and playerb to use when no name supplied
      m_statsSelection->addItem("PERCENTAGE DISTRIBUTION");
      m_statsSelection->addItem("HEIGHT GRAPH");
      m_statsSelection->addItem("TRAJECTORY GRAPH");
      m_statsSelection->addItem("VELOCITY MAP");
      m_statsSelection->addItem("RPM MAP");

      QObject::connect(m_statsSelection,SIGNAL(currentIndexChanged(int)),this,SLOT(setWhichStatToView(int)));
      m_toolbarPlayer->addWidget(m_statsSelection);


      m_chooseCourtSideLabel = new QLabel(m_toolbarPlayer);
      m_chooseCourtSideLabel->setText("Court Sides");
      m_toolbarPlayer->addWidget(m_chooseCourtSideLabel);

      m_quadSelection = new QComboBox(m_toolbarPlayer);
      //slider->setMinimum(100);
      //slider->setMaximum(255);
      m_quadSelection->addItem("TOP LEFT"); // try making this into player names..initialize it by def
                                            // to playera and playerb to use when no name supplied
      m_quadSelection->addItem("BOTTOM LEFT");
      m_quadSelection->addItem("BOTTOM RIGHT");
      m_quadSelection->addItem("TOP RIGHT");
      m_quadSelection->addItem("FULL COURT");

      m_quadSelection->setCurrentIndex(4);

      //QObject::connect(m_quadSelection,SIGNAL(currentIndexChanged(int)),m_vz,SLOT(setWhichQuadToView(int)));
      m_toolbarPlayer->addWidget(m_quadSelection);
      m_quadSelection->setEnabled(false);

      m_toolbarPlayer->setEnabled(false);



    this->addToolBar(m_toolbarOrig);
    this->addToolBar(m_toolbarImport);
    // adds the debug toolbar and is hidden by default
    this->addToolBar(m_toolbarPlayer);
    this->addToolBar(m_toolbar);



    setImportToolBarVisibility(false);
    setLiveSetUpToolBarVisibility(false);
    setLiveStartUpToolBarVisibility(false);
    setPlayerToolBarVisibility(false);

    m_mdiArea = new QMdiArea;
    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(m_mdiArea);
    this->setWindowTitle(QString("Kinect Ball Tracking/Statistics Generation"));

    m_vz = 0;
    m_processingUtility = 0;
    m_trackingInBackground = 0;
    m_trackingInForeground = 0;
    m_rgb = 0;
    m_depthVz = 0;
    m_playerA = m_playerB = 0;
}



MainWindow::~MainWindow()
{
    // DELETE EVERYTHING

    delete m_vz;
    delete m_processingUtility;
    delete m_trackingInBackground;
    delete m_trackingInForeground;
    delete m_depthVz;
    delete m_rgb;
    delete m_playerA;
    delete m_playerB;
}


void MainWindow::keyPressEvent(
                               QKeyEvent *_event
                              )
{
    qDebug()<<"key event\n";
    // this method is called every time the main window recives a key eventcompiler error .
    // we then switch on the key value and set the camera in the StatsVisualization
    switch (_event->key())
    {
        case Qt::Key_Escape : m_kinect->shutDownKinect(); QApplication::exit(EXIT_SUCCESS); break;
        case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
        case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
        default : break;
    }
}

void MainWindow::resizeEvent ( QResizeEvent * _event )
{
  Q_UNUSED(_event);
}

void MainWindow::toggleMasterRigCheck(int _inputState)
{
    if(_inputState)
    {
        m_toolbar->setHidden(false);
    }
    else
    {
        m_toolbar->setHidden(true);

    }
}

void MainWindow::setTrackingFlag()
{
    if(m_kinect->getBounds())
    {
        if(m_kinect->getTrackingFlag() == false)
        {
            m_startTracking->setText("STOP TRACKING");

            // disable process stats button if enabled
            processStats->setDisabled(true);

        }
        else
        {
            m_startTracking->setText("START TRACKING");

            // enable process stats button
            processStats->setDisabled(false);
            // we are going to process the data as the tracking has now been stopped
            //processingUtility->processTrackingData();
        }
        m_kinect->toggleTracking();

    }
    else
    {
        // bounds not yet defined...warn user to define bounds and close the RGB window for performance
        QString warningString("Please define field of play before proceeding. ");
        warningString += "To define field of play, use your mouse to click and drag in the RGB window.";
        warningString += "When finished, click Right mouse button to see the defined bounds in the Stats window.";
        warningString += "Once done, close the RGB window for better performance.";
        QMessageBox * warning = new QMessageBox(QMessageBox::Warning,QString("FIELD OF PLAY WARNING"),warningString,QMessageBox::Ok);
        warning->setModal(true);
        warning->setText(warningString);
        warning->show();
    }
}

void MainWindow::setWhichStatToView(int _whichStat)
{
    // if % distrbn selected, display quad selection
    if(_whichStat == IMPACT_PERCENTAGE_DISTRIBUTION_VIEW)
    {
        m_quadSelection->setEnabled(true);


    }
    else
    {
        m_quadSelection->setEnabled(false);
    }


    // if the user has selected ht map view,velocity view
    // or rpm view or trajectory view
    // and if the player selection is BOTH,
    // then we deault the player to PLAYERA
    // and change the value of the player selection
    // combo box to PLAYERA
    if((_whichStat == IMPACT_HEIGHT_MAP_VIEW) || (_whichStat == VELOCITY_VIEW) ||
       (_whichStat == RPM_VIEW) || (_whichStat == TRAJECTORY_VIEW))
    {
        if(m_playerSelection->currentIndex() == BOTH)
        {
            m_playerSelection->setCurrentIndex(PLAYERA);

            m_vz->setWhoseStatToView(PLAYERA);

        }
        else
        {
            m_vz->setWhoseStatToView(m_playerSelection->currentIndex());
        }
    }

       m_vz->setWhichStatToView(_whichStat);


}

void MainWindow::setWhoseStatToView(int _whoseStat)
{

    // if the user has selected ht map view,velocity view
    // or rpm view
    // and if the player selection is BOTH,
    // then we deault the player to PLAYERA
    // and change the value of the player selection
    // combo box to PLAYERA
    if((m_statsSelection->currentIndex() == IMPACT_HEIGHT_MAP_VIEW) ||
            (m_statsSelection->currentIndex() == VELOCITY_VIEW)  ||
            (m_statsSelection->currentIndex() == RPM_VIEW) ||
            (m_statsSelection->currentIndex() == TRAJECTORY_VIEW))
    {
        if(_whoseStat == BOTH)
        {
            m_playerSelection->setCurrentIndex(PLAYERA);
            m_vz->setWhoseStatToView(PLAYERA);
        }
        else
        {
            m_vz->setWhoseStatToView(_whoseStat);
        }

    }
    else
    {
        m_vz->setWhoseStatToView(_whoseStat);
    }
}

void MainWindow::readFile()
{

    killExistingWindows();
    buildImportWindowSetUp();

    m_vz = new StatsVisualization(this,true);
    m_vz->makeCurrent();
    //QMdiSubWindow *importedStatsWindow = new QMdiSubWindow;
    ExtendedQMdiSubWindow *importedStatsWindow = new ExtendedQMdiSubWindow(this);
    importedStatsWindow->setWidget(m_vz);
    importedStatsWindow->setAccessibleName("Statsvz");
    importedStatsWindow->setAttribute(Qt::WA_DeleteOnClose);
    importedStatsWindow->setWindowTitle("Imported Statistical Output");
    importedStatsWindow->resize(640,480);
    m_mdiArea->addSubWindow(importedStatsWindow);

    // WE NEED TO FIRST CALL THIS SHOW METHOD
    // AS THIS WILL CREATE THE VALID GL CONTEXT
    // THRU INITIALIZEGL WHICH IS NEEDED FOR CREATING VAOS
    // AND PRIMS IN THE READFILE FUNCTION WHICH FOLLOWS THIS
    importedStatsWindow->show();

    m_toolbarPlayer->setEnabled(true);
    QObject::connect(m_quadSelection,SIGNAL(currentIndexChanged(int)),m_vz,SLOT(setWhichQuadToView(int)));

    // call the read function
    // pass in the input file name by querying the file input value

    QString fileName = QFileDialog::getOpenFileName(this, "Open File","archive");


    // check if filename is valid, if so call read
    if(!(fileName.isEmpty()))
    {
        m_vz->readFile(fileName);
    }
}

void MainWindow::processTrackingData()
{
    // do not forget to disable the process stats by default and enable it only
    // when tracking has been stopped
    // again is start tracking is clicked, process stats has to be enabled

    m_processingUtility->processTrackingData();
    m_vz->updateDisplay();

}


void MainWindow::buildImportWindowSetUp()
{
    killExistingWindows();

    setLiveSetUpToolBarVisibility(false);
    setLiveStartUpToolBarVisibility(false);
    // 2 more calls to set visualisation toolbars to hide

    // enable import bar visibility for read button
    setImportToolBarVisibility(true);

    setPlayerToolBarVisibility(true);
}


void MainWindow::buildLiveWindowSetUp()
{

    killExistingWindows();

    setImportToolBarVisibility(false);

    setPlayerToolBarVisibility(true);

//    if(!(m_liveWindowBuilt))
//    {
    m_kinect=KinectInterface::instance();

    if(m_kinect == 0)
    {
        QString warningString("The application could not find a Kinect connected.Please Connet the device and click \"Yes\" when ready.");
        QMessageBox msgBox(QMessageBox::Warning,QString("DEVICE CONNECTION WARNING"),warningString,QMessageBox::Yes | QMessageBox::Cancel);

        //returns the button clicked value
        int ret = msgBox.exec();

        //User get input from returned value (ret). you can handle it here.
        switch (ret)
        {
            case QMessageBox::Yes:
            {
                // yes was clicked
                buildLiveWindowSetUp();
                break;
            }
            case QMessageBox::Cancel:
            {
                // cancel was clicked
                break;
            }
            default:
                // should never be reached
                break;
        }
        // if the user presses cancel we shud return
        return;
     }


    // we need to reset the bounds defined finally flag here
    // so that the user is allowed to redfine the bounds
    // once he comes back to live or setup sections of the tool
    KinectInterface *kinect = KinectInterface::instance();
    kinect->setBoundsDefined(false);

     // Time to create our players and processing object

     m_playerA = new PlayerData();
     m_playerB = new PlayerData();

     m_processingUtility = new BallPointsProcessing(m_playerA,m_playerB);
     m_trackingInBackground = new BallTrackingUtility(m_processingUtility);


     // this needs to be created before any other subwindows as
     // mysteriously the ngl text appears in only one window which is created first
     // and we need to display ngl text within this window
     // for percentage statistics
     m_vz = new StatsVisualization(this,m_playerA,m_playerB);
     m_vz->makeCurrent();
     ExtendedQMdiSubWindow *statsWindow = new ExtendedQMdiSubWindow(this);
     statsWindow->setWidget(m_vz);
     statsWindow->setAccessibleName("Statsvz");
     statsWindow->setAttribute(Qt::WA_DeleteOnClose);
     statsWindow->setWindowTitle("Statistical Output");
     statsWindow->resize(640,480);
     m_mdiArea->addSubWindow(statsWindow);
     statsWindow->show();


     m_rgb = new RGBboundsVisualization(this);
     m_rgb->makeCurrent();
     ExtendedQMdiSubWindow *rgbBoundsWindow = new ExtendedQMdiSubWindow(this);
     rgbBoundsWindow->setWidget(m_rgb);
     rgbBoundsWindow->setAccessibleName("Rgbvz");
     rgbBoundsWindow->setAttribute(Qt::WA_DeleteOnClose);
     rgbBoundsWindow->setWindowTitle("RGB Output");
     rgbBoundsWindow->resize(640,480);
     m_mdiArea->addSubWindow(rgbBoundsWindow);
     rgbBoundsWindow->show();



     m_toolbarPlayer->setEnabled(true);
     QObject::connect(m_depthVolumeLowerThresh,SIGNAL(valueChanged(double)),m_kinect,SLOT(setDepthVolumeLower(double)));
     QObject::connect(m_depthVolumeHigherThresh,SIGNAL(valueChanged(double)),m_kinect,SLOT(setDepthVolumeHigher(double)));
     QObject::connect(m_binLowerSpinBox,SIGNAL(valueChanged(double)),m_trackingInBackground,SLOT(setBinaryLowerThresh(double)));
     QObject::connect(m_cannyLowerSpinBox,SIGNAL(valueChanged(double)),m_trackingInBackground,SLOT(setCannyLowerThresh(double)));
     QObject::connect(m_cannyHigherSpinBox,SIGNAL(valueChanged(double)),m_trackingInBackground,SLOT(setCannyHigherThresh(double)));
     QObject::connect(m_contourAreaThresh,SIGNAL(valueChanged(double)),m_trackingInBackground,SLOT(setContourAreaThresh(double)));
     QObject::connect(m_impactPtMinDepth,SIGNAL(valueChanged(double)),m_processingUtility,SLOT(setBallImpactDepthMinThreshold(double)));
     QObject::connect(m_impactPtMaxDepth,SIGNAL(valueChanged(double)),m_processingUtility,SLOT(setBallImpactDepthMaxThreshold(double)));
     QObject::connect(m_diffAreaThresh,SIGNAL(valueChanged(double)),m_trackingInBackground,SLOT(setDifferenceInAreaThresh(double)));
     QObject::connect(m_ellipseSize,SIGNAL(sliderMoved(int)),m_trackingInBackground,SLOT(setStructuringElementSize(int)));
     QObject::connect(m_blurSize,SIGNAL(sliderMoved(int)),m_trackingInBackground,SLOT(setBlurSize(int)));
     QObject::connect(m_dialation,SIGNAL(sliderMoved(int)),m_trackingInBackground,SLOT(setNumerOfDialtions(int)));
     QObject::connect(m_quadSelection,SIGNAL(currentIndexChanged(int)),m_vz,SLOT(setWhichQuadToView(int)));
     QObject::connect(m_kinectToPlayArea,SIGNAL(valueChanged(double)),m_vz,SLOT(setKinectToPlayAreaDistance(double)));
     QObject::connect(m_diameterOfBall,SIGNAL(valueChanged(double)),m_vz,SLOT(setDiameterOfBallInMeters(double)));

     QObject::connect(m_writeFile,SIGNAL(clicked()),m_vz,SLOT(writeFile()));


     m_writeFile->setEnabled(true);


//     m_liveWindowBuilt = true;

//    }

    setLiveSetUpToolBarVisibility(true);
    setLiveStartUpToolBarVisibility(true);

}


void MainWindow::buildTrackingVzWindowSetUp()
{
    killExistingWindows();

    setImportToolBarVisibility(false);

    setPlayerToolBarVisibility(false);

    if(m_writeFile->isEnabled())
    {
        m_writeFile->setEnabled(false);
    }

    //if(!(m_liveWindowBuilt))
    //{
    m_kinect=KinectInterface::instance();

    if(m_kinect == 0)
    {
        QString warningString("The application could not find a Kinect connected.Please Connet the device and click \"Yes\" when ready.");
        QMessageBox msgBox(QMessageBox::Warning,QString("DEVICE CONNECTION WARNING"),warningString,QMessageBox::Yes | QMessageBox::Cancel);

        //returns the button clicked value
        int ret = msgBox.exec();

        //User get input from returned value (ret). you can handle it here.
        switch (ret)
        {
            case QMessageBox::Yes:
            {
                // yes was clicked
                buildTrackingVzWindowSetUp();
                break;
            }
            case QMessageBox::Cancel:
            {
                // cancel was clicked
                break;
            }
            default:
                // should never be reached
                break;
        }
        // if the user presses cancel we shud return
        return;
     }


    // we need to reset the bounds defined finally flag here
    // so that the user is allowed to redfine the bounds
    // once he comes back to live or setup sections of the tool
    KinectInterface *kinect = KinectInterface::instance();
    kinect->setBoundsDefined(false);


    m_rgb = new RGBboundsVisualization(this);
    m_rgb->makeCurrent();
    ExtendedQMdiSubWindow *rgbBoundsWindow = new ExtendedQMdiSubWindow(this);
    rgbBoundsWindow->setWidget(m_rgb);
    rgbBoundsWindow->setAccessibleName("Rgbvz");
    rgbBoundsWindow->setAttribute(Qt::WA_DeleteOnClose);
    rgbBoundsWindow->setWindowTitle("RGB Output");
    rgbBoundsWindow->resize(640,480);
    m_mdiArea->addSubWindow(rgbBoundsWindow);
    rgbBoundsWindow->show();

    m_depthVz = new DepthDebugVisualization(this);
    m_depthVz->makeCurrent();
    ExtendedQMdiSubWindow *depthWindow = new ExtendedQMdiSubWindow(this);
    depthWindow->setWidget(m_depthVz);
    depthWindow->setAccessibleName("Depthlive");
    depthWindow->setAttribute(Qt::WA_DeleteOnClose);
    depthWindow->setWindowTitle("Sliced Depth Realtime Output");
    depthWindow->resize(640,480);
    m_mdiArea->addSubWindow(depthWindow);
    depthWindow->show();

    m_trackingInForeground = new BallTrackingVisualization(this);
    m_trackingInForeground->makeCurrent();
    ExtendedQMdiSubWindow *trackingForeGround = new ExtendedQMdiSubWindow(this);
    trackingForeGround->setWidget(m_trackingInForeground);
    trackingForeGround->setAccessibleName("Foregroundlive");
    trackingForeGround->setAttribute(Qt::WA_DeleteOnClose);
    trackingForeGround->setWindowTitle("Ball Tracking RealTime Output");
    trackingForeGround->resize(640,480);
    m_mdiArea->addSubWindow(trackingForeGround);
    trackingForeGround->show();


     m_toolbarPlayer->setEnabled(true);
     QObject::connect(m_depthVolumeLowerThresh,SIGNAL(valueChanged(double)),m_kinect,SLOT(setDepthVolumeLower(double)));
     QObject::connect(m_depthVolumeHigherThresh,SIGNAL(valueChanged(double)),m_kinect,SLOT(setDepthVolumeHigher(double)));
     QObject::connect(m_binLowerSpinBox,SIGNAL(valueChanged(double)),m_trackingInForeground,SLOT(setBinaryLowerThresh(double)));
     QObject::connect(m_cannyLowerSpinBox,SIGNAL(valueChanged(double)),m_trackingInForeground,SLOT(setCannyLowerThresh(double)));
     QObject::connect(m_cannyHigherSpinBox,SIGNAL(valueChanged(double)),m_trackingInForeground,SLOT(setCannyHigherThresh(double)));
     QObject::connect(m_contourAreaThresh,SIGNAL(valueChanged(double)),m_trackingInForeground,SLOT(setContourAreaThresh(double)));
     QObject::connect(m_diffAreaThresh,SIGNAL(valueChanged(double)),m_trackingInForeground,SLOT(setDifferenceInAreaThresh(double)));
     QObject::connect(m_ellipseSize,SIGNAL(sliderMoved(int)),m_trackingInForeground,SLOT(setStructuringElementSize(int)));
     QObject::connect(m_blurSize,SIGNAL(sliderMoved(int)),m_trackingInForeground,SLOT(setBlurSize(int)));
     QObject::connect(m_dialation,SIGNAL(sliderMoved(int)),m_trackingInForeground,SLOT(setNumerOfDialtions(int)));
     setLiveSetUpToolBarVisibility(true);
     setLiveStartUpToolBarVisibility(true);

}

// following 3 functions shud be called with appropriate bool value

void MainWindow::setLiveStartUpToolBarVisibility(bool _visibility)
{
    // start,clear,process,write containing box
    m_toolbarOrig->setVisible(_visibility);
}

void MainWindow::setLiveSetUpToolBarVisibility(bool _visibility)
{
    // start,clear,process,write containing box
    m_toolbar->setVisible(_visibility);
}

void MainWindow::setImportToolBarVisibility(bool _visibility)
{
    // read button containing box visibility
    m_toolbarImport->setVisible(_visibility);
}

void MainWindow::setPlayerToolBarVisibility(bool _visibility)
{
    m_toolbarPlayer->setVisible(_visibility);
}



void MainWindow::killExistingWindows()
{
    // since we have our own Mdi window
    // we have overridden the closeevent
    // and deleted the containing widget
    // and the associated pointers to avoid memory leaks
    // for all windows..this is done
    // in the ExtendedQMdiSubWindow class

    if(m_vz)
    {
        // this command will close the existing window
        // as we are going to create a new stats window if
        // user clicks read button
        if(m_vz->nativeParentWidget())
        {
            m_vz->nativeParentWidget()->close();
        }
    }

    if(m_rgb)
    {
        // this command will close the existing window

        if(m_rgb->nativeParentWidget())
        {
            m_rgb->nativeParentWidget()->close();
        }
    }


    if(m_depthVz)
    {
        // this command will close the existing window

        if(m_depthVz->nativeParentWidget())
        {
            m_depthVz->nativeParentWidget()->close();
        }

    }

    if(m_trackingInForeground)
    {
        // this command will close the existing window

        if(m_trackingInForeground->nativeParentWidget())
        {
            m_trackingInForeground->nativeParentWidget()->close();
        }
    }



    if(m_trackingInBackground)
    {
        delete m_trackingInBackground;
        m_trackingInBackground = 0;
    }

    if(m_playerA)
    {
        delete m_playerA;
        m_playerA = 0;
    }

    if(m_playerB)
    {
        delete m_playerB;
        m_playerB = 0;
    }

    if(m_processingUtility)
    {
        delete m_processingUtility;
        m_processingUtility = 0;
    }
}

void MainWindow::deleteCurrentStatsvz()
{
    delete m_vz;
    m_vz = 0;
}

void MainWindow::deleteCurrentTrackingvz()
{
    delete m_trackingInForeground;
    m_trackingInForeground = 0;
}

void MainWindow::deleteCurrentRGBvz()
{
    delete m_rgb;
    m_rgb = 0;
}

void MainWindow::deleteCurrentDepthvz()
{
    delete m_depthVz;
    m_depthVz = 0;

}


