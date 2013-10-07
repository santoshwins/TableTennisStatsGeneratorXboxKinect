#ifndef BALLTRACKINGUTILITY_H
#define BALLTRACKINGUTILITY_H

// must be included after our stuff becuase GLEW needs to be first
#include "KinectInterface.h"
#include "ScreenQuad.h"
#include "QuadCurveFitUtility.h"
#include <ngl/Text.h>

#include <QTime>
#include <QEvent>
#include <QResizeEvent>

/// @file BallTrackingUtility.h
/// @brief ball track utility
/// @author Santosh
/// @version 1.0
/// @date 16/08/13
/// @class BallTrackingUtility
/// @brief ball track utility

class BallPointsProcessing;

class BallTrackingUtility:public QObject
{
    Q_OBJECT        // must include this if you use Qt signals/slots

    private :


        cv::Mat m_rgb, m_gray;

        cv::Mat m_nextBuffer,m_prevBuffer, m_diffBuffer, m_bufferDepthRaw, m_bufferDepthRaw1;
        float m_depthinm[640*480];
        QTime m_timer;
        std::vector<cv::Point3f> m_ballPoints, m_processedBallPts;
        std::vector<cv::Point2f> m_pointList, m_pointListXZ, m_pointListForCV;
        std::vector<cv::Moments> m_momentList;

        QuadCurveFitUtility *m_quadFit , *m_quadFitXZ, *m_quadFitForCV, *m_quadFitHermite;
        QuadCurveFitUtility *m_quadFitX,*m_quadFitY,*m_quadFitZ;

        cv::Mat canny_output;
        std::vector<std::vector<cv::Point> > contours;
        std::vector<int> hull;
        std::vector<cv::Vec4i> defects;
        std::vector<cv::Vec4i> hierarchy;

        double area,rectArea;

        std::vector<cv::Moments> mu;
        std::vector<cv::Point2f> mc;

        double m_binaryLowerThresh,m_binaryMaxVal,m_cannyLowerThresh,m_cannyHigherThresh,m_contourAreaThresh,m_diffAreaThresh;
        int m_ellipsesize,m_blurSize,m_noOfTimesToDialate, m_masterRigCheck;

        std::vector<cv::Point2f> m_playerAImpactData;

        QTimer *m_updateTimer;

        BallPointsProcessing *m_ballPointProcessing;

    protected:


    public :
        /// @brief Constructor for RGBboundsVisualization
        /// @param [in] _parent the parent window to create the GL context in
        BallTrackingUtility(
                           );

        BallTrackingUtility(BallPointsProcessing *_inputBallProcessingUtility);
        ~BallTrackingUtility();

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

        void convertToWorldFromScreen(float _iScreen, float _jScreen, float _zDepth);

        void fillparametricCoords();



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

  void trackBall();

};

#endif // BALLTRACKINGUTILITY_H
