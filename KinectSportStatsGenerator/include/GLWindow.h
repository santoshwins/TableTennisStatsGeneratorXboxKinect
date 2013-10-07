#ifndef __GL_WINDOW_H__
#define __GL_WINDOW_H__


#include "ngl/Camera.h"
#include "ngl/Colour.h"
#include "ngl/TransformStack.h"
// must be included after our stuff becuase GLEW needs to be first
#include <QtOpenGL>
#include <QTime>
#include "libfreenect.hpp"
#include "Kinect.h"
#include <QTime>
/// @file GLWindow.h
/// @brief a basic Qt GL window class for ngl demos
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/10/10
/// Revision History :
/// Initial Version 10/10/10 (Binary day ;-0 )
/// @class GLWindow
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
class GLWindow : public QGLWidget
{
Q_OBJECT        // must include this if you use Qt signals/slots
public :
  /// @brief Constructor for GLWindow
  /// @param [in] _parent the parent window to create the GL context in
  GLWindow(
           QWidget *_parent
          );
	~GLWindow();
//	inline Kinect* getDevice(){return device;}
		 void incrementAngle();
		 void decrementAngle();
		 void zeroAngle();
private :

  /// @brief used to store the x rotation mouse value
  int m_spinXFace;
  /// @brief used to store the y rotation mouse value
  int m_spinYFace;
  /// @brief flag to indicate if the mouse button is pressed when dragging
  bool m_rotate;
  /// @brief the previous x mouse value
  int m_origX;
  /// @brief the previous y mouse value
  int m_origY;
	void LoadVertexMatrix();
	void LoadRGBMatrix();
	void drawPointCloud();
	// @brief Our Camera
  ngl::Camera *m_cam;
	ngl::Camera *m_pcam;

	 std::vector<uint8_t> depth;
	 std::vector<uint8_t> rgb;
	 /// @brief flag for the fps timer
		 int m_fpsTimer;
		 /// @brief the fps to draw
		 int m_fps;
		 int m_frames;
		 QTime m_timer;
  /// @brief transformation stack for the gl transformations etc
  ngl::TransformStack m_transformStack;

	float m_angle;
	GLuint gl_depth_tex;
	GLuint gl_rgb_tex;

//Freenect::Freenect<Kinect> freenect;
//	Kinect* device;

protected:

  /// @brief  The following methods must be implimented in the sub class
  /// this is called when the window is created
  /// \note these are part of the Qt API so can't be changed to the coding standard
  /// so it can't be called initializeGL )
  void initializeGL();

  /// @brief this is called whenever the window is re-sized
  /// @param[in] _w the width of the resized window
  /// @param[in] _h the height of the resized window
  /// \note these are part of the Qt API so can't be changed to the coding standard
  /// so it can't be called resizeGL )
  void resizeGL(
                const int _w,
                const int _h
               );
  /// @brief this is the main gl drawing routine which is called whenever the window needs to
  // be re-drawn
  void paintGL();

private :
  /// @brief this method is called every time a mouse is moved
  /// @param _event the Qt Event structure
  /// \note these are part of the Qt API so can't be changed to the coding standard
  /// so it can't be called MouseMoveEvent )

  void mouseMoveEvent (
                       QMouseEvent * _event
                      );
  /// @brief this method is called everytime the mouse button is pressed
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  /// \note these are part of the Qt API so can't be changed to the coding standard
  /// so it can't be called MousePressEvent )

  void mousePressEvent (
                        QMouseEvent *_event
                       );

  /// @brief this method is called everytime the mouse button is released
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  /// \note these are part of the Qt API so can't be changed to the coding standard
  /// so it can't be called MousePressEvent )
  void mouseReleaseEvent (
                          QMouseEvent *_event
                         );
  /// @brief called when the timer is triggered

  void timerEvent(
                    QTimerEvent *_event
                   );


};

#endif
