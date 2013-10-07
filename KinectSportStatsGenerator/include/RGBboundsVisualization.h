#ifndef RGB_WINDOW_H__
#define RGB_WINDOW_H__


// must be included after our stuff becuase GLEW needs to be first
#include "KinectInterface.h"
#include<ngl/Text.h>


#include <QTime>
#include <QEvent>
#include <QResizeEvent>
#include <ScreenQuad.h>

/// @file RGBboundsVisualization.h
/// @brief a basic Qt GL window class for ngl demos
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/10/10
/// Revision History :
/// Initial Version 10/10/10 (Binary day ;-0 )
/// @class RGBboundsVisualization
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file


class RGBboundsVisualization : public QGLWidget
{
Q_OBJECT        // must include this if you use Qt signals/slots
public :
  /// @brief Constructor for RGBboundsVisualization
  /// @param [in] _parent the parent window to create the GL context in
  RGBboundsVisualization(QWidget *_parent);
  ~RGBboundsVisualization();

private :


    cv::Mat m_rgb;

    QTime m_timer;
    ScreenQuad *m_screen;


    // frame rate by GUNDU

    /// @brief our font
    //ngl::Text *m_text;
    /// @brief calculate framerate
    //QTime currentTime;

    cv::Rect m_selectionBox;
    bool m_drawingStatus;


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
