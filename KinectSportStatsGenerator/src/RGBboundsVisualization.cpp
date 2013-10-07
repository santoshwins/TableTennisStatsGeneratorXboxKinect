#include "RGBboundsVisualization.h"
#include <iostream>
#include <ngl/NGLInit.h>
#include <libfreenect.hpp>
#include <libfreenect_sync.h>
#include <Mutex.h>
#include <QDebug>
#include <KinectInterface.h>
#include <ngl/ShaderLib.h>


//---------------------------------------------------------------------------------------------------------------------
RGBboundsVisualization::RGBboundsVisualization(
                      QWidget *_parent
                    ) : QGLWidget(new CreateCoreGLContext(QGLFormat::defaultFormat()), _parent)

{

  // set this widget to have the initial keyboard focus
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());
  startTimer(5); // increasing this timer causes the stats window to lose its information

  m_rgb = cvCreateMat(640,480,CV_8UC3);
  m_drawingStatus = false;
}

// This virtual function is called once before the first call to paintGL() or resizeGL(),
//and then once whenever the widget has been assigned a new QGLContext.
// This function should set up any required OpenGL context rendering flags, defining display lists, etc.

//----------------------------------------------------------------------------------------------------------------------
void RGBboundsVisualization::initializeGL()
{
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing

    ngl::NGLInit *Init = ngl::NGLInit::instance();
    Init->initGlew();
    // now we have valid GL can create shaders etc
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
    shader->createShaderProgram("TextureRGB");

    shader->attachShader("TextureVertex",ngl::VERTEX);
    shader->attachShader("TextureFragment",ngl::FRAGMENT);
    shader->loadShaderSource("TextureVertex","shaders/TextureVert.glsl");
    shader->loadShaderSource("TextureFragment","shaders/TextureFrag.glsl");

    shader->compileShader("TextureVertex");
    shader->compileShader("TextureFragment");
    shader->attachShaderToProgram("TextureRGB","TextureVertex");
    shader->attachShaderToProgram("TextureRGB","TextureFragment");

    shader->linkProgramObject("TextureRGB");
    (*shader)["TextureRGB"]->use();
    shader->setShaderParam1i("tex",0);

    m_screen = new ScreenQuad(640,480,"TextureRGB");


    //frame rate GUNDU
//    m_text=new ngl::Text(QFont("Arial",14));
//    currentTime = currentTime.currentTime();

}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget has been resized.
// The new size is passed in width and height.
void RGBboundsVisualization::resizeGL(
                        int _w,
                        int _h
                       )
{
  glViewport(0,0,_w,_h);

   /*m_text->setScreenSize(_w,_h);*/
}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
void RGBboundsVisualization::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
    (*shader)["TextureRGB"]->use();

    // calculate the framerate //frame rate GUNDU
//    QTime newTime = currentTime.currentTime();
//    int msecsPassed = currentTime.msecsTo(newTime);
//    currentTime = newTime;



    KinectInterface *kinect=KinectInterface::instance();
    kinect->getRGB(m_rgb);

    cv::rectangle(m_rgb, cv::Point(m_selectionBox.x, m_selectionBox.y), cv::Point(m_selectionBox.x+m_selectionBox.width,m_selectionBox.y+m_selectionBox.height),
                    cv::Scalar(255,0,0),5 );

    m_screen->draw(&m_rgb);


   /* QString text;
    // framerate GUNDU
    m_text->setColour(1,1,1);
    text.sprintf("framerate is %d",(int)(1000.0/msecsPassed));
    m_text->renderText(10,50,text);*/
}

//----------------------------------------------------------------------------------------------------------------------
void RGBboundsVisualization::mouseMoveEvent (QMouseEvent * _event )
{
 // Q_UNUSED(_event);

    if( m_drawingStatus ){
                    m_selectionBox.width = _event->x()-m_selectionBox.x;
                    m_selectionBox.height = _event->y()-m_selectionBox.y;
                }

  updateGL();
}


//----------------------------------------------------------------------------------------------------------------------
void RGBboundsVisualization::mousePressEvent ( QMouseEvent * _event  )
{
  //Q_UNUSED(_event);

//    std::cout<<_event->x()<<"\n";
//    std::cout<<_event->y()<<"\n";
   // std::cout<<_event->globalX()<<"\n";
   // std::cout<<_event->globalY()<<"\n";

     if(_event->buttons() == Qt::LeftButton)
     {

                m_drawingStatus = true;
                m_selectionBox = cv::Rect(_event->x(), _event->y(), 0, 0 );



     }
     else if(_event->buttons() == Qt::RightButton)
     {
         KinectInterface *kinect=KinectInterface::instance();

         m_drawingStatus = false;



         kinect->setBoxObject(m_selectionBox.x,m_selectionBox.y,m_selectionBox.width,m_selectionBox.height);

//         std::cout<<m_selectionBox.x<<"\n";
//         std::cout<<m_selectionBox.y<<"\n";
//         std::cout<<m_selectionBox.width<<"\n";
//         std::cout<<m_selectionBox.height<<"\n";

     }
    updateGL();

   // cv::line(m_rgb,cv::Point(0,0),cv::Point(630,470),cv::Scalar(255,0,0),4);

}

//----------------------------------------------------------------------------------------------------------------------
void RGBboundsVisualization::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
 // Q_UNUSED(_event);

    if(_event->buttons() == Qt::LeftButton)
    {
        m_drawingStatus = false;
        if( m_selectionBox.width < 0 ){
            m_selectionBox.x += m_selectionBox.width;
            m_selectionBox.width *= -1;
        }
        if( m_selectionBox.height < 0 ){
            m_selectionBox.y += m_selectionBox.height;
            m_selectionBox.height *= -1;
        }



    }


    updateGL();
}

void RGBboundsVisualization::timerEvent( QTimerEvent *_event)
{
    Q_UNUSED(_event);
    // re-draw GL
    updateGL();
}

RGBboundsVisualization::~RGBboundsVisualization()
{
    if(m_screen)
    {
        delete m_screen;
    }
}



