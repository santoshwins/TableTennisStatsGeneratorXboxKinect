#ifndef LUMINANCEWINDOW_H__
#define LUMINANCEWINDOW_H__


// must be included after our stuff becuase GLEW needs to be first
#include "KinectInterface.h"
#include "ScreenQuad.h"

#include <QTime>
#include <QEvent>
#include <QResizeEvent>

/// @file DepthDebugVisualization.h
/// @brief ball track visualization
/// @author Santosh
/// @version 1.0
/// @date 16/08/13
/// @class DepthDebugVisualization
/// @brief to visualize depth data


class DepthDebugVisualization : public QGLWidget
{
Q_OBJECT        // must include this if you use Qt signals/slots
public :
  /// @brief Constructor for DepthDebugVisualization
  /// @param [in] _parent the parent window to create the GL context in
  DepthDebugVisualization(
             QWidget *_parent
             );
  ~DepthDebugVisualization();

private :


    cv::Mat m_depth;

    QTime m_timer;

    ScreenQuad *m_screen;




protected:

  /// @brief  The following methods must be implimented in the sub class
  /// this is called when the window is created
  void initializeGL();

  /// @brief this is called whenever the window is re-sized
  /// @param[in] _w the width of the resized window
  /// @param[in] _h the height of the resized window
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

  void mouseMoveEvent (
                       QMouseEvent * _event
                      );
  /// @brief this method is called everytime the mouse button is pressed
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure

  void mousePressEvent (
                        QMouseEvent *_event
                       );

  /// @brief this method is called everytime the mouse button is released
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  void mouseReleaseEvent (
                          QMouseEvent *_event
                         );
  /// @brief called when the timer is triggered

  void timerEvent(
                    QTimerEvent *_event
                   );

};

#endif
