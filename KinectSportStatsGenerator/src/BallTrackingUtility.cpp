#include "BallTrackingUtility.h"
#include "BallPointsProcessing.h"

#include <iostream>
#include <libfreenect.hpp>
#include <libfreenect_sync.h>
#include <Mutex.h>
#include <QDebug>
#include <KinectInterface.h>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <QTimer>
#include <QObject>

void thresh_callback(int, void* );

bool compareXX(cv::Point2f i, cv::Point2f j){ return (i.x < j.x);}

bool compareXXHermite(cv::Point3f i, cv::Point3f j){ return (i.x < j.x);}

bool compareXX3D(cv::Point3f i, cv::Point3f j){ return (i.x < j.x);}


BallTrackingUtility::BallTrackingUtility()
{

}

//----------------------------------------------------------------------------------------------------------------------
BallTrackingUtility::BallTrackingUtility(BallPointsProcessing *_inputBallProcessingUtility):m_ballPointProcessing(_inputBallProcessingUtility)

{

  m_rgb = cvCreateMat(480,640,CV_8UC3);

  m_gray = cvCreateMat(480,640,CV_8UC1);

  m_nextBuffer = cvCreateMat(480,640,CV_8UC1);
  m_prevBuffer = cvCreateMat(480,640,CV_8UC1);
  m_diffBuffer = cvCreateMat(480,640,CV_8UC1);

  m_bufferDepthRaw=cvCreateMat(480,640,CV_8UC1);
  m_bufferDepthRaw1=cvCreateMat(480,640,CV_8UC1);

  m_binaryLowerThresh = 100.0;
  m_binaryMaxVal = 255.0;
  m_cannyLowerThresh = 100.0;
  m_cannyHigherThresh = 300.0;
  m_contourAreaThresh = 120.0;
  m_diffAreaThresh = 30.0;
  m_ellipsesize = 2;
  m_blurSize = 5;
  m_noOfTimesToDialate = 2;

  m_masterRigCheck = 0;

  for( unsigned int i = 0 ; i < 640*480 ; ++i)
  {
      m_depthinm[i] = 0;
  }

  // all these are separate lists filled by separate calls to interpolate functions
  m_ballPoints.clear();
  m_momentList.clear();
  m_processedBallPts.clear();

  area = rectArea = 0.0;
  m_updateTimer = new QTimer(this);

  connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(trackBall()));
  m_updateTimer->start(1);

}


//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
void BallTrackingUtility::trackBall()
{


    KinectInterface *kinect=KinectInterface::instance();

    cv::Rect _boundsCheck = kinect->getBoxObject();

 if(kinect->getTrackingFlag())
 {
    static int _frameCount = 1;

    float depth = 0.0;

    kinect->getDepthRaw(m_bufferDepthRaw,m_depthinm);

    cv::blur(m_bufferDepthRaw,m_bufferDepthRaw1,cv::Size(m_blurSize,m_blurSize));
    // to reduce noise, smotthing has been done

    if(_frameCount == 1)
    {
        m_bufferDepthRaw1.copyTo(m_prevBuffer);
    }

    m_bufferDepthRaw1.copyTo(m_nextBuffer);
    cv::absdiff(m_nextBuffer,m_prevBuffer,m_diffBuffer);
    // has gpu variant // bg subtraction by frame differencing ...GMM method seems good through forums if at all we need
    m_nextBuffer.copyTo(m_prevBuffer);
    m_diffBuffer.copyTo(m_gray);

    cv::threshold(m_gray,m_gray,m_binaryLowerThresh,m_binaryMaxVal,CV_THRESH_BINARY_INV);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(m_ellipsesize,m_ellipsesize));
    cv::dilate(m_gray,m_gray,element,cv::Point(-1,-1),m_noOfTimesToDialate);
    cv::erode(m_gray,m_gray,element,cv::Point(-1,-1),m_noOfTimesToDialate);

    /// Detect edges using canny
    cv::Canny( m_gray, canny_output, m_cannyLowerThresh, m_cannyHigherThresh, 3 ); // maintiain upper::lower ratio as 2:1 or 3:1 for better results
                                                                                   // as suggested by CANNY
    /// Find contours
    cv::findContours( canny_output, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) ); // methods to be decided


    for( int i = 0; i < contours.size(); i++ )
    {
        /* cv::drawContours( drawing, contours, i, cv::Scalar(255,0,0),-1, 8, hierarchy, 0, cv::Point() );*/ // -- DEV PURPOSE
        area = cv::contourArea(contours[i], false);
        cv::RotatedRect r = cv::minAreaRect(contours[i]);

        if (fabs(area) < m_contourAreaThresh) //my notes: this value is more important in reducing the
            //false positives and filters out those false positives by preventing them
            // from getting processed further..provide in UI
        {
            continue;
        }
        else
        {
            // do nothing
        }

       if (fabs(fabs(area) - (r.size.width * r.size.height * 3.14 / 4)) < m_diffAreaThresh)
       {
           m_momentList.push_back(cv::moments( contours[i], false ));
           float _xMomentsTemp = static_cast<float>(cv::moments( contours[i], false ).m10/cv::moments( contours[i], false ).m00);
           float _yMomentsTemp = static_cast<float>(cv::moments( contours[i], false ).m01/cv::moments( contours[i], false ).m00);
           int depthIndex = (int)((640 - _xMomentsTemp) + (640 * _yMomentsTemp));
           depth = m_depthinm[depthIndex] * 0.001;

           if(depth != 0)
           {
            if((_xMomentsTemp >= _boundsCheck.x) && (_xMomentsTemp <= _boundsCheck.x + _boundsCheck.width) &&
                    (_yMomentsTemp >= _boundsCheck.y) && (_yMomentsTemp <= _boundsCheck.y + _boundsCheck.height))
            {


                std::cout<<"XMoments:"<<_xMomentsTemp<<"\n";
                m_ballPointProcessing->setTrackedPoints(cv::Point3f(_xMomentsTemp,_yMomentsTemp,depth));

                convertToWorldFromScreen(_xMomentsTemp,_yMomentsTemp,depth);

             }
            }

          }
        }


        _frameCount++;
  }
 else
 {
     // tracking not yet started...do nothing


 }


}

//----------------------------------------------------------------------------------------------------------------------
void BallTrackingUtility::mouseMoveEvent ( QMouseEvent * _event)
{
    Q_UNUSED(_event);
}


//----------------------------------------------------------------------------------------------------------------------
void BallTrackingUtility::mousePressEvent (QMouseEvent * _event )
{
    Q_UNUSED(_event);

}

//----------------------------------------------------------------------------------------------------------------------
void BallTrackingUtility::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  Q_UNUSED(_event);

}

void BallTrackingUtility::timerEvent( QTimerEvent *_event )
{
    Q_UNUSED(_event);
    // re-draw GL
    //updateGL();

    trackBall();

}

void BallTrackingUtility::convertToWorldFromScreen(float _iScreen, float _jScreen, float _zDepth)
{


    float x = (_iScreen/640 - 0.5) * _zDepth * 1.111467f;
    float y = (0.5 - _jScreen/480) * _zDepth * 0.833599f;

     m_ballPoints.push_back(cv::Point3f(-(y),(1 - _zDepth),-x));

}



BallTrackingUtility::~BallTrackingUtility()
{

}




void BallTrackingUtility::setBinaryLowerThresh(double _lowerThresh)
{
    m_binaryLowerThresh = _lowerThresh;
    //updateGL();
}

void BallTrackingUtility::setStructuringElementSize(int _size)
{
    m_ellipsesize = _size;
    //updateGL();
}

void BallTrackingUtility::setBlurSize(int _size)
{
    m_blurSize = _size;
    //updateGL();
}

void BallTrackingUtility::setCannyLowerThresh(double _lowerThresh)
{
    m_cannyLowerThresh = _lowerThresh;
    //updateGL();
}


void BallTrackingUtility::setCannyHigherThresh(double _higherThresh)
{
    m_cannyHigherThresh = _higherThresh;
    //updateGL();
}


void BallTrackingUtility::setNumerOfDialtions(int _numberOfDialtions)
{
    m_noOfTimesToDialate = _numberOfDialtions;
    //updateGL();
}

void BallTrackingUtility::setContourAreaThresh(double _contourAreaThresh)
{
    m_contourAreaThresh = _contourAreaThresh;
    //updateGL();
}

void BallTrackingUtility::setDifferenceInAreaThresh(double _diffAreaThresh)
{
    m_diffAreaThresh = _diffAreaThresh;
    //updateGL();
}

void BallTrackingUtility::setMasterRigCheck(int _masterRigCheck)
{
    m_masterRigCheck = _masterRigCheck;
    //updateGL();
}


