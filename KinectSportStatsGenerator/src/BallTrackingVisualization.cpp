#include "BallTrackingVisualization.h"
#include "StatsVisualization.h"

#include <iostream>
#include <libfreenect.hpp>
#include <libfreenect_sync.h>
#include <Mutex.h>
#include <QDebug>
#include <KinectInterface.h>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>


//----------------------------------------------------------------------------------------------------------------------
BallTrackingVisualization::BallTrackingVisualization(QWidget *_parent) : QGLWidget( new CreateCoreGLContext(QGLFormat::defaultFormat()), _parent )

{

  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());
  startTimer(20);
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
  m_cannyHigherThresh = 200.0;
  m_contourAreaThresh = 100.0;
  m_diffAreaThresh = 10.0;
  m_ellipsesize = 2;
  m_blurSize = 5;
  m_noOfTimesToDialate = 2;

  m_masterRigCheck = 0;

  for( unsigned int i = 0 ; i < 640*480 ; ++i)
  {
      m_depthinm[i] = 0;
  }


  m_momentList.clear();

  area = rectArea = 0.0;
}

// This virtual function is called once before the first call to paintGL() or resizeGL(),
//and then once whenever the widget has been assigned a new QGLContext.
// This function should set up any required OpenGL context rendering flags, defining display lists, etc.

//----------------------------------------------------------------------------------------------------------------------
void BallTrackingVisualization::initializeGL()
{

  glEnable(GL_DEPTH_TEST);


    //
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  Init->initGlew();

  // now we have valid GL can create shaders etc
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  shader->createShaderProgram("TextureTrack");

  shader->attachShader("TextureVertex",ngl::VERTEX);
  shader->attachShader("TextureFragment",ngl::FRAGMENT);
  shader->loadShaderSource("TextureVertex","shaders/TextureVert.glsl");
  shader->loadShaderSource("TextureFragment","shaders/TextureFrag.glsl");

  shader->compileShader("TextureVertex");
  shader->compileShader("TextureFragment");
  shader->attachShaderToProgram("TextureTrack","TextureVertex");
  shader->attachShaderToProgram("TextureTrack","TextureFragment");

  shader->linkProgramObject("TextureTrack");
  (*shader)["TextureTrack"]->use();
  shader->setShaderParam1i("tex",0);

  m_screen = new ScreenQuad(640,480,"TextureTrack");


}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget has been resized.
// The new size is passed in width and height.
void BallTrackingVisualization::resizeGL(
                        int _w,
                        int _h
                       )
{
  glViewport(0,0,_w,_h);

}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
void BallTrackingVisualization::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    KinectInterface *kinect=KinectInterface::instance();

    cv::Rect _boundsCheck = kinect->getBoxObject();

    if(kinect->getTrackingFlag())
    {


        static int _frameCount = 1;
       float depth = 0.0;

        kinect->getDepthRaw(m_bufferDepthRaw,m_depthinm);

        // commented for gpu trial

        cv::blur(m_bufferDepthRaw,m_bufferDepthRaw1,cv::Size(m_blurSize,m_blurSize)); // to reduce noise, smotthing has been done
        /*  cv::gpu::blur(m_bufferDepthRawGpu,m_bufferDepthRawGpu1,cv::Size(m_blurSize,m_blurSize));*/

        if(_frameCount == 1)
        {
            m_bufferDepthRaw1.copyTo(m_prevBuffer);
        }

        m_bufferDepthRaw1.copyTo(m_nextBuffer);

        // commented for gpu trial

        cv::absdiff(m_nextBuffer,m_prevBuffer,m_diffBuffer); // has gpu variant // bg subtraction by frame differencing ...GMM method seems good through forums if at all we need
        /*cv::gpu::absdiff(m_nextBufferGpu,m_prevBufferGpu,m_diffBufferGpu);*/

        m_nextBuffer.copyTo(m_prevBuffer);

        // commented for gpu trial

        m_diffBuffer.copyTo(m_gray);
        /*m_diffBuffer.copyTo(m_grayGpu);*/

        // commented for gpu tiral
        cv::threshold(m_gray,m_gray,m_binaryLowerThresh,m_binaryMaxVal,CV_THRESH_BINARY_INV); // nned to decide the thresh factors and the thresh method
        cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(m_ellipsesize,m_ellipsesize)); // need to decide this size
        cv::dilate(m_gray,m_gray,element,cv::Point(-1,-1),m_noOfTimesToDialate); // try swapping this with below and try increasing iterations
        cv::erode(m_gray,m_gray,element,cv::Point(-1,-1),m_noOfTimesToDialate);

        /*cv::gpu::threshold(m_grayGpu,m_grayGpu,m_binaryLowerThresh,m_binaryMaxVal,CV_THRESH_BINARY_INV);
        cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(m_ellipsesize,m_ellipsesize)); // need to decide this size
        cv::gpu::dilate(m_grayGpu,m_grayGpu,element,cv::Point(-1,-1),m_noOfTimesToDialate); // try swapping this with below and try increasing iterations
        cv::gpu::erode(m_grayGpu,m_grayGpu,element,cv::Point(-1,-1),m_noOfTimesToDialate);*/

        // commented for gpu trial

        // Detect edges using canny
        cv::Canny( m_gray, canny_output, m_cannyLowerThresh, m_cannyHigherThresh, 3 ); // maintiain upper::lower ratio as 2:1 or 3:1 for better results
                                                                                     // as suggested by CANNY
        /*  cv::gpu::Canny(m_grayGpu, canny_outputGpu, m_cannyLowerThresh, m_cannyHigherThresh, 3);*/
        // commented for gpu trial

        // Find contours
        cv::findContours( canny_output, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) ); // methods to be decided
        /*cv::findContours( canny_outputGpu, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) ); // methods to be decided*/


        cv::Mat drawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 );


        for( int i = 0; i < contours.size(); i++ )
        {

            /* cv::drawContours( drawing, contours, i, cv::Scalar(255,0,0),-1, 8, hierarchy, 0, cv::Point() );*/ // -- DEV PURPOSE

            area = cv::contourArea(contours[i], false);

            cv::RotatedRect r = cv::minAreaRect(contours[i]);



            if (fabs(area) < m_contourAreaThresh) //my notes: this value is more important in reducing the false positives and filters out those false positives by preventing them
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
                   if((_xMomentsTemp >= _boundsCheck.x) && (_xMomentsTemp <= _boundsCheck.x + _boundsCheck.width)
                           && (_yMomentsTemp >= _boundsCheck.y) && (_yMomentsTemp <= _boundsCheck.y + _boundsCheck.height))
                   {
                       cv::ellipse(drawing,r,cv::Scalar(0,255,0),-1,8);
                       std::cout<<"Rect center x:"<<r.center.x<<"\n";
                   }
                 }

             }

          }

       _frameCount++;

       m_screen->draw(&drawing);

     }
     else
     {
        // tracking not yet started...do nothing
     }

}

//----------------------------------------------------------------------------------------------------------------------
void BallTrackingVisualization::mouseMoveEvent ( QMouseEvent * _event)
{
    Q_UNUSED(_event);
}


//----------------------------------------------------------------------------------------------------------------------
void BallTrackingVisualization::mousePressEvent (QMouseEvent * _event )
{
    Q_UNUSED(_event);

}

//----------------------------------------------------------------------------------------------------------------------
void BallTrackingVisualization::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  Q_UNUSED(_event);

}

void BallTrackingVisualization::timerEvent( QTimerEvent *_event )
{
    Q_UNUSED(_event);
    // re-draw GL
    updateGL();

}


BallTrackingVisualization::~BallTrackingVisualization()
{
    if(m_screen)
    {
        delete m_screen;
    }
}



void BallTrackingVisualization::setBinaryLowerThresh(double _lowerThresh)
{
    m_binaryLowerThresh = _lowerThresh;
    updateGL();
}

void BallTrackingVisualization::setStructuringElementSize(int _size)
{
    m_ellipsesize = _size;
    updateGL();
}

void BallTrackingVisualization::setBlurSize(int _size)
{
    m_blurSize = _size;
    updateGL();
}

void BallTrackingVisualization::setCannyLowerThresh(double _lowerThresh)
{
    m_cannyLowerThresh = _lowerThresh;
    updateGL();
}


void BallTrackingVisualization::setCannyHigherThresh(double _higherThresh)
{
    m_cannyHigherThresh = _higherThresh;
    updateGL();
}


void BallTrackingVisualization::setNumerOfDialtions(int _numberOfDialtions)
{
    m_noOfTimesToDialate = _numberOfDialtions;
    updateGL();
}

void BallTrackingVisualization::setContourAreaThresh(double _contourAreaThresh)
{
    m_contourAreaThresh = _contourAreaThresh;
    updateGL();
}

void BallTrackingVisualization::setDifferenceInAreaThresh(double _diffAreaThresh)
{
    m_diffAreaThresh = _diffAreaThresh;
    updateGL();
}

void BallTrackingVisualization::setMasterRigCheck(int _masterRigCheck)
{
    m_masterRigCheck = _masterRigCheck;
    updateGL();
}

