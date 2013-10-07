#include "GLWindow.h"
#include <iostream>
#include "ngl/Camera.h"
#include "ngl/Colour.h"
#include "ngl/Light.h"
#include "ngl/Matrix.h"
#include "ngl/Transformation.h"
#include "ngl/TransformStack.h"
#include "ngl/Material.h"
#include "ngl/NGLInit.h"
#include "ngl/Obj.h"
#include "ngl/VBOPrimitives.h"
#include "ngl/ShaderManager.h"
#include "libfreenect.hpp"
#include "libfreenect_sync.h"
#include "Mutex.h"
#include <QDebug>
//----------------------------------------------------------------------------------------------------------------------
GLWindow::GLWindow(
                   QWidget *_parent
                  ) :
                    QGLWidget(_parent)
{

  // set this widget to have the initial keyboard focus
 // setFocus();
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());
  // Now set the initial GLWindow attributes to default values
  // Roate is false
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  m_fpsTimer =startTimer(0);
  m_fps=0;
  m_frames=0;
  m_timer.start();
	//device = &freenect.createDevice(0);
	//device->startVideo();
	//device->startDepth();
	//depth.resize(640*480*4);
	//rgb.resize(640*480*4);
	m_fpsTimer =startTimer(0);
		m_fps=0;
		m_frames=0;
}

// This virtual function is called once before the first call to paintGL() or resizeGL(),
//and then once whenever the widget has been assigned a new QGLContext.
// This function should set up any required OpenGL context rendering flags, defining display lists, etc.

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::initializeGL()
{

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing

	glEnable(GL_DEPTH_TEST);
	 ngl::NGLInit *Init = ngl::NGLInit::instance();
	 Init->initGlew();


	 glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	 glClearDepth(1.0);
		 //glDepthFunc(GL_LESS);
		 //glDisable(GL_DEPTH_TEST);
		 //glEnable(GL_BLEND);
		 glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		 glShadeModel(GL_SMOOTH);
		 glGenTextures(1, &gl_depth_tex);
		 glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		 glGenTextures(1, &gl_rgb_tex);
		 glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		 glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
			 // enable depth testing for drawing
				glEnable(GL_DEPTH_TEST);
				// Now we will create a basic Camera from the graphics library
				// This is a static camera so it only needs to be set once
				// First create Values for the camera position
				ngl::Vector From(2,2,2);
				ngl::Vector To(0,0,0);
				ngl::Vector Up(0,1,0);
				Init->initGlew();
				Init->initVBO();
				m_cam= new ngl::Camera(From,To,Up,ngl::PERSPECTIVE);
				// set the shape using FOV 45 Aspect Ratio based on Width and Height
				// The final two are near and far clipping planes of 0.5 and 10
				m_cam->setShape(60,(float)720.0/576.0,0.5,10,ngl::PERSPECTIVE);

				m_pcam= new ngl::Camera(ngl::Vector(0,0,8),ngl::Vector(0,0,0),ngl::Vector(0,1,0),ngl::PERSPECTIVE);
				m_pcam->setShape(40,(float)2.5/2.0,0.1,120,ngl::PERSPECTIVE);

				//m_pcam->setOrthoParams(-1,1,-1,1,1,10.0); //setOrthoParams(	0,0,100,100,1,10);
				// set the shape using FOV 45 Aspect Ratio based on Width and Height
				// The final two are near and far clipping planes of 0.5 and 10


				// now to load the shader and set the values
				// grab an instance of shader manager
				ngl::ShaderManager *shader=ngl::ShaderManager::instance();
				// load a frag and vert shaders
				shader->loadShader("gl3xTest","shaders/Vertex.vs","shaders/Fragment.fs");
				// set this as the active shader
				shader->useShader("gl3xTest");
				// now pass the modelView and projection values to the shader
				shader->setShaderParamFromMatrix("gl3xTest","ViewMatrix",m_cam->getModelView());
				shader->setShaderParamFromMatrix("gl3xTest","projectionMatrix",m_cam->getProjection());
				// the shader will use the currently active material and light0 so set them
				shader->setShaderParam1i("gl3xTest","Normalize",true);
					shader->setShaderParam1i("gl3xTest","numLightsEnabled",3);
					shader->setShaderParam3f("gl3xTest","eye",m_cam->getEye()[0],m_cam->getEye()[1],m_cam->getEye()[2]);

				// load a frag and vert shaders for colour no shading
					shader->loadShader("Colour","shaders/Colour.vs","shaders/Colour.fs");
					// set this as the active shader
					shader->useShader("Colour");
					shader->setShaderParamFromMatrix("Colour","ViewMatrix",m_pcam->getModelView());
					shader->setShaderParamFromMatrix("Colour","projectionMatrix",m_pcam->getProjection());


				ngl::Material m(ngl::GOLD);
				m.use();
				ngl::Light *L1 = new ngl::Light(ngl::Vector(5,12,0,1),ngl::Colour(1,1,1,1),ngl::LIGHTLOCAL);
				L1->enable();
				ngl::VBOPrimitives *prim=ngl::VBOPrimitives::instance();
				prim->createVBOQuadPlane("plane",2.5,2,20,20,ngl::Vector(0,1,0));
}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget has been resized.
// The new size is passed in width and height.
void GLWindow::resizeGL(
                        int _w,
                        int _h
                       )
{
  glViewport(0,0,_w,_h);
	m_cam->setShape(45,(float)_w/_h,0.5,10,ngl::PERSPECTIVE);
		ngl::ShaderManager *shader=ngl::ShaderManager::instance();
	shader->useShader("gl3xTest");
	shader->setShaderParamFromMatrix("gl3xTest","projectionMatrix",m_cam->getProjection());
	shader->useShader("Colour");
	shader->setShaderParamFromMatrix("Colour","projectionMatrix",m_pcam->getProjection());

/*
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho (0, 1280, 480, 0, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
*/
}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
void GLWindow::paintGL()
{

//		device->getDepth(depth);
//		device->getRGB(rgb);




  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_TEXTURE_2D);
	// grab an instance of the shader manager
		ngl::ShaderManager *shader=ngl::ShaderManager::instance();
		shader->useShader("gl3xTest");
		// clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Rotation based on the mouse position for our global
		// transform
		ngl::Transformation trans;
		trans.setRotation(m_spinXFace,m_spinYFace,0);
		// set this in the TX stack
		m_transformStack.setGlobal(trans);
		// now set this value in the shader for the current ModelMatrix
		shader->setShaderParamFromMatrix("gl3xTest","ModelMatrix",m_transformStack.getCurrAndGlobal().getMatrix());
		// get the VBO instance and draw the built in teapot
		ngl::VBOPrimitives *prim=ngl::VBOPrimitives::instance();



		m_transformStack.pushTransform();
		{
			//m_transformStack.getCurrentTransform().setScale(4,4,4);
			shader->setShaderParamFromMatrix("gl3xTest","ModelMatrix",m_transformStack.getCurrAndGlobal().getMatrix());
			prim->draw("teapot");
		} // and before a pop
		m_transformStack.popTransform();


		glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, &depth[0]);
		glDisable(GL_LIGHTING);
		shader->useShader("Colour");

		m_transformStack.pushTransform();
		{
			m_transformStack.getCurrentTransform().setPosition(-3,2,0);
			m_transformStack.getCurrentTransform().setRotation(90.0f,0,180);
			shader->setShaderParamFromMatrix("Colour","ModelMatrix",m_transformStack.getCurrentTransform().getMatrix());
			prim->draw("plane");
		} // and before a pop
		m_transformStack.popTransform();



		glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, &rgb[0]);

		m_transformStack.pushTransform();
		{
			m_transformStack.getCurrentTransform().setPosition(-3,-0.5,0);
			m_transformStack.getCurrentTransform().setRotation(90.0f,0,180);
			shader->setShaderParamFromMatrix("Colour","ModelMatrix",m_transformStack.getCurrentTransform().getMatrix());
			prim->draw("plane");
		} // and before a pop
		m_transformStack.popTransform();

		// calculate and draw FPS
			++m_frames;
			glUseProgramObjectARB(0);
			glColor3f(1,1,1);
			QFont font;
			font.setPointSize(20);
			QString text=QString("Kinect Demo %1 fps").arg(m_fps);
			renderText(500,20,text,font);
			double x,y,z;
			device->getState().getAccelerometers(&x,&y,&z);
			text=QString("Accel %1 %2 %3").arg(x).arg(y).arg(z);
			renderText(400,40,text,font);

}

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mouseMoveEvent (
                               QMouseEvent * _event
                              )
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    m_spinYFace = ( m_spinYFace + (_event->x() - m_origX) ) % 360 ;
    m_spinXFace = ( m_spinXFace + (_event->y() - m_origY) ) % 360 ;
    m_origX = _event->x();
    m_origY = _event->y();
  }
  // re-draw GL
  updateGL();
}


//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mousePressEvent (
                                QMouseEvent * _event
                               )
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mouseReleaseEvent (
                                  QMouseEvent * _event
                                 )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
}

void GLWindow::timerEvent(
                          QTimerEvent *_event
                         )
{

		if(_event->timerId() == m_fpsTimer)
		{
			if( m_timer.elapsed() > 1000.0)
			{
				m_fps=m_frames;
				m_frames=0;
				m_timer.restart();
			}
		 }
			// re-draw GL
	updateGL();

}

GLWindow::~GLWindow()
{
	device->stopVideo();
	device->stopDepth();
}



void  GLWindow::incrementAngle()
{
	m_angle++;
	if(m_angle > 30)
	{
		m_angle = 30;
	}
	qDebug()<<"Tilt"<<m_angle;
	device->setTiltDegrees(m_angle);
}

void GLWindow::decrementAngle()
{
	m_angle--;
	if(m_angle < -30)
	{
		m_angle = -30;
	}
	device->setTiltDegrees(m_angle);

}

void GLWindow::zeroAngle()
{
	m_angle=0;
	device->setTiltDegrees(m_angle);

}




// Do the projection from u,v,depth to X,Y,Z directly in an opengl matrix
// These numbers come from a combination of the ros kinect_node wiki, and
// nicolas burrus' posts.
	void GLWindow::LoadVertexMatrix()
{
		float fx = 594.21f;
		float fy = 591.04f;
		float a = -0.0030711f;
		float b = 3.3309495f;
		float cx = 339.5f;
		float cy = 242.7f;
		GLfloat mat[16] = {
				1/fx,     0,  0, 0,
				0,    -1/fy,  0, 0,
				0,       0,  0, a,
				-cx/fx, cy/fy, -1, b
		};
		glMultMatrixf(mat);
}


// This matrix comes from a combination of nicolas burrus's calibration post
// and some python code I haven't documented yet.
void GLWindow::LoadRGBMatrix()
{
		float mat[16] = {
				5.34866271e+02,   3.89654806e+00,   0.00000000e+00,   1.74704200e-02,
				-4.70724694e+00,  -5.28843603e+02,   0.00000000e+00,  -1.22753400e-02,
				-3.19670762e+02,  -2.60999685e+02,   0.00000000e+00,  -9.99772000e-01,
				-6.98445586e+00,   3.31139785e+00,   0.00000000e+00,   1.09167360e-02
		};
		glMultMatrixf(mat);
}



void GLWindow::drawPointCloud()
{

		uint32_t ts;

		static unsigned int indices[480][640];
		static short xyz[480][640][3];
		int i,j;
		for (i = 0; i < 480; i++) {
				for (j = 0; j < 640; j++) {
						xyz[i][j][0] = j;
						xyz[i][j][1] = i;
						xyz[i][j][2] = depth[i*640+j];
						indices[i][j] = i*640+j;
				}
		}

		glLoadIdentity();

		glPushMatrix();
	//	glScalef(zoom,zoom,1);
		glTranslatef(0,0,-3.5);
		//glRotatef(rotangles[0], 1,0,0);
		//glRotatef(rotangles[1], 0,1,0);
		glTranslatef(0,0,1.5);

		LoadVertexMatrix();

		// Set the projection from the XYZ to the texture image
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScalef(1/640.0f,1/480.0f,1);
		LoadRGBMatrix();
		LoadVertexMatrix();
		glMatrixMode(GL_MODELVIEW);

		glPointSize(1);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_SHORT, 0, xyz);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(3, GL_SHORT, 0, xyz);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, &rgb[0]);

		glPointSize(2.0f);
		glDrawElements(GL_POINTS, 640*480, GL_UNSIGNED_INT, indices);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
}


