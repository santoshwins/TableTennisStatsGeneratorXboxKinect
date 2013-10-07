#include "DepthDebugVisualization.h"
#include <iostream>
#include <libfreenect.hpp>
#include <libfreenect_sync.h>
#include <Mutex.h>
#include <QDebug>
#include <KinectInterface.h>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"

//----------------------------------------------------------------------------------------------------------------------
DepthDebugVisualization::DepthDebugVisualization(QWidget *_parent) : QGLWidget( new CreateCoreGLContext(QGLFormat::defaultFormat()), _parent )

{

  // set this widget to have the initial keyboard focus
 // setFocus();
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());
  startTimer(20);
  m_depth = cvCreateMat(480,640,CV_8UC1);

}

// This virtual function is called once before the first call to paintGL() or resizeGL(),
//and then once whenever the widget has been assigned a new QGLContext.
// This function should set up any required OpenGL context rendering flags, defining display lists, etc.

//----------------------------------------------------------------------------------------------------------------------
void DepthDebugVisualization::initializeGL()
{
  //
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  Init->initGlew();

  // now we have valid GL can create shaders etc
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  shader->createShaderProgram("TextureDepth");

  shader->attachShader("TextureVertex",ngl::VERTEX);
  shader->attachShader("TextureFragment",ngl::FRAGMENT);
  shader->loadShaderSource("TextureVertex","shaders/TextureVert.glsl");
  shader->loadShaderSource("TextureFragment","shaders/LumFrag.glsl");

  shader->compileShader("TextureVertex");
  shader->compileShader("TextureFragment");
  shader->attachShaderToProgram("TextureDepth","TextureVertex");
  shader->attachShaderToProgram("TextureDepth","TextureFragment");

  shader->linkProgramObject("TextureDepth");
  (*shader)["TextureDepth"]->use();
  shader->setShaderParam1i("tex",0);

  m_screen = new ScreenQuad(640,480,"TextureDepth");
  // set this to draw depth textures
  m_screen->setTextureMode(GL_DEPTH_COMPONENT );

  //frame rate GUNDU
  //m_text=new ngl::Text(QFont("Arial",14));
  //currentTime = currentTime.currentTime();
}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget has been resized.
// The new size is passed in width and height.
void DepthDebugVisualization::resizeGL(
                        int _w,
                        int _h
                       )
{
  glViewport(0,0,_w,_h);

  //m_text->setScreenSize(_w,_h);
}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
void DepthDebugVisualization::paintGL()
{
    //int temp = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    KinectInterface *kinect=KinectInterface::instance();
    // this is the raw data will need to be processed
    kinect->getDepthSliced(m_depth);

    m_screen->draw(&m_depth);
}

//----------------------------------------------------------------------------------------------------------------------
void DepthDebugVisualization::mouseMoveEvent ( QMouseEvent * _event)
{
    Q_UNUSED(_event);
}


//----------------------------------------------------------------------------------------------------------------------
void DepthDebugVisualization::mousePressEvent (QMouseEvent * _event )
{
    Q_UNUSED(_event);

}

//----------------------------------------------------------------------------------------------------------------------
void DepthDebugVisualization::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  Q_UNUSED(_event);

}

void DepthDebugVisualization::timerEvent( QTimerEvent *_event )
{
    Q_UNUSED(_event);
    // re-draw GL
    updateGL();

}

DepthDebugVisualization::~DepthDebugVisualization()
{
    if(m_screen)
    {
        delete m_screen;
    }
}



