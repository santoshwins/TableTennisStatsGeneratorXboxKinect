#ifndef DEPTH_WINDOW_H__
#define DEPTH_WINDOW_H__


// must be included after our stuff becuase GLEW needs to be first
#include "KinectInterface.h"
#include "ScreenQuad.h"
#include "QuadCurveFitUtility.h"

#include <QTime>
#include <QEvent>
#include <QResizeEvent>

//#include <opencv2/gpu/gpu.hpp>

/// @file BallTrackingVisualization.h
/// @brief ball track visualization
/// @author Santosh
/// @version 1.0
/// @date 16/08/13
/// @class BallTrackingVisualization
/// @brief ball track visualization as green circles just for visualization of real time tracking

class StatsVisualization;

enum VIDEOMODE{RGB,DEPTH};

class BallTrackingVisualization : public QGLWidget
{
Q_OBJECT        // must include this if you use Qt signals/slots
public :
  /// @brief Constructor for RGBboundsVisualization
  /// @param [in] _parent the parent window to create the GL context in
  BallTrackingVisualization(
             QWidget *_parent
             );
  ~BallTrackingVisualization();


  std::vector<cv::Point3f>& getBallPts(); // not used by anyone currently

  void setVisualizationInstance(StatsVisualization *_inputPtr);


private :

    cv::Mat m_rgb, m_gray;

    cv::Mat m_nextBuffer,m_prevBuffer, m_diffBuffer, m_bufferDepthRaw, m_bufferDepthRaw1;
    float m_depthinm[640*480];
    ScreenQuad *m_screen;
    std::vector<cv::Point3f> m_ballPoints, m_processedBallPts;
    cv::Vec6f m_fittedPoints;

    cv::Point pt1,pt2;
    float t,d; // line equation variables

    std::vector<cv::Point2f> m_pointList, m_pointListXZ, m_pointListForCV;

    std::vector<cv::Moments> m_momentList;

    cv::Mat canny_output;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<int> hull;
    std::vector<cv::Vec4i> defects;
    std::vector<cv::Vec4i> hierarchy;

    double area,rectArea;

    std::vector<cv::Moments> mu;
    std::vector<cv::Point2f> mc;
    double m_binaryLowerThresh,m_binaryMaxVal,m_cannyLowerThresh,m_cannyHigherThresh;
    double m_contourAreaThresh,m_diffAreaThresh;
    int m_ellipsesize,m_blurSize,m_noOfTimesToDialate, m_masterRigCheck;

    std::vector<cv::Point2f> m_playerAImpactData;





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


public slots:
  void setBinaryLowerThresh(double _lowerThresh);



  void setStructuringElementSize(int _size);


  void setBlurSize(int _size);


  void setCannyLowerThresh(double _lowerThresh);



  void setCannyHigherThresh(double _higherThresh);



  void setNumerOfDialtions(int _numberOfDialtions);


  void setContourAreaThresh(double _contourAreaThresh);


  void setDifferenceInAreaThresh(double _diffAreaThresh);


  void setMasterRigCheck(int _masterRigCheck);

};

#endif
