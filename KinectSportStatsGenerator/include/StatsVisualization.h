#ifndef VISUALIZATIONWINDOW_H
#define VISUALIZATIONWINDOW_H

#include "ScreenQuad.h"
#include "PlayerData.h"

#include <ngl/Camera.h>
#include <ngl/Colour.h>
#include <ngl/TransformStack.h>
#include <ngl/ShaderLib.h>
#include <ngl/VertexArrayObject.h>
#include <ngl/Text.h>


#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <iostream>
#include <string>

// must be included after our stuff becuase GLEW needs to be first
#include <QtOpenGL>

/// @file StatsVisualization.h
/// @brief stats visualization
/// @author Santosh
/// @version 1.0
/// @date 16/08/13
/// @class StatsVisualization
/// @brief complete stats vz window


typedef enum
{
    PLAYERA,
    PLAYERB,
    BOTH
}WHOSE_STATS;

typedef enum
{
    IMPACT_POINTS_VIEW,
    IMPACT_PERCENTAGE_DISTRIBUTION_VIEW,
    IMPACT_HEIGHT_MAP_VIEW,
    TRAJECTORY_VIEW,
    VELOCITY_VIEW,
    RPM_VIEW

}WHICH_STAT;

struct VertData
{
  //GLfloat u; // tex cords
  //GLfloat v; // tex cords
  GLfloat nx; // normal
  GLfloat ny;
  GLfloat nz;
  GLfloat x; // position
  GLfloat y;
  GLfloat z;
};

class StatsVisualization : public QGLWidget
{
Q_OBJECT        // must include this if you use Qt signals/slots
public :

    //def constructor
    StatsVisualization();

  /// @brief Constructor for StatsVisualization
  /// @param [in] parent the parent window to create the GL context in
  StatsVisualization(
           QWidget *_parent, PlayerData *_playerA, PlayerData *_playerB
          );


  // this constructor is called when displaying imported data rather than live data
  // so it does not need to know the players...
  // all the data is filled up from the input file
  StatsVisualization(
           QWidget *_parent, bool _importFlag
          );

  ~StatsVisualization();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief override the keyPressEvent inherited from QObject so we can handle key presses.
  /// @param [in] _event the event to process

    void processKey(
                     QKeyEvent *_event
                  );

    void fillBallPoints(std::vector<cv::Point3f> &_inputBallPtsList);

    void fillImpactPoints(cv::Point2f _impactPt);

    void updateDisplay();

private :

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to store the x rotation mouse value
  //----------------------------------------------------------------------------------------------------------------------
  int m_spinXFace;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to store the y rotation mouse value
  //----------------------------------------------------------------------------------------------------------------------
  int m_spinYFace;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief flag to indicate if the mouse button is pressed when dragging
  //----------------------------------------------------------------------------------------------------------------------
  bool m_rotate;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief flag to indicate if the Right mouse button is pressed when dragging
  //----------------------------------------------------------------------------------------------------------------------
  bool m_translate;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the previous x mouse value
  //----------------------------------------------------------------------------------------------------------------------
  int m_origX;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the previous y mouse value
  //----------------------------------------------------------------------------------------------------------------------
  int m_origY;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the previous x mouse value for Position changes
  //----------------------------------------------------------------------------------------------------------------------
  int m_origXPos;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the previous y mouse value for Position changes
  //----------------------------------------------------------------------------------------------------------------------
  int m_origYPos;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the model position for mouse movement
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 m_modelPos;
  //----------------------------------------------------------------------------------------------------------------------

  /// @brief Our Camera
  ngl::Camera *m_cam;
   /// @brief our transform stack for drawing the teapots
  ngl::TransformStack m_transformStack;
  /// @brief the curve to use
  //ngl::BezierCurve *m_curve;

  int m_ballPtsFilled;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the vertex array object pointer
  //----------------------------------------------------------------------------------------------------------------------
  ngl::VertexArrayObject *m_vao;
  //int m_tempVao; // just to check if trajectory data s working fine, change this to m_vao1 of vertex object

  std::vector<cv::Point3f> m_inputBallPtsList;

  ScreenQuad *m_2DScreen;

  int m_updateTimer;

  //ngl::ShaderLib *shaderTex,*shader;

  // stats array

  std::vector<cv::Point> m_playerAImpactData;
  std::vector<cv::Point> m_playerBImpactData;

  float m_percentageABL,m_percentageATL,m_percentageATR,m_percentageABR;

  float m_percentageBBL,m_percentageBTL,m_percentageBTR,m_percentageBBR;

  std::vector<cv::Point> m_quadSpecABL,m_quadSpecATL,m_quadSpecATR,m_quadSpecABR;

  std::vector<cv::Point> m_quadSpecBBL,m_quadSpecBTL,m_quadSpecBTR,m_quadSpecBBR;

  std::vector<std::vector<cv::Point3f> > m_playerAInterpPtsMappedToRallies;
  std::vector<std::vector<cv::Point3f> > m_playerBInterpPtsMappedToRallies;

  std::vector<float> m_playerATrajectoryFromFile;
  std::vector<float> m_playerBTrajectoryFromFile;

  // vertices data
  std::vector<VertData> m_dataVertsA, m_dataVertsB;


  std::vector<float> m_speedAData,m_speedBData;
  std::vector<cv::Point3f> m_playerAImpactPtsAllRalliesIn3D,m_playerBImpactPtsAllRalliesIn3D;
  std::vector<float> m_playerARPM,m_playerBRPM;


  cv::Rect m_fieldOfPlay;
  float m_finalCourtWidthWorld,m_finalCourtDepthWorld;
  int m_width,m_height,m_midTopX,m_midTopY,m_midBottomX,m_midBottomY,m_midLeftX;
  int m_midLeftY,m_midRightX,m_midRightY;


  PlayerData *m_playerA,*m_playerB;

  bool m_impactPtsFilled;
  bool m_speedMapBuilt,m_rpmDataBuilt,m_quadMapBuilt,m_trajectoryBuilt;

  WHOSE_STATS m_whichPlayer;
  WHICH_STAT m_whichStat;

  WHICH_QUADRANT m_whichQuad;

  bool m_3dFlag;

  // just used to define where the actual field of play is in the world space
  // so that will be useful to render the 3D data with respect to this scale
  // from 0,0,0
  float m_kinectToPlayAreaInMeters;
  float m_diameterOfBAllInMeters;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief a simple light use to illuminate the screen
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 m_lightPos;

  bool m_importFlag;

  /// @brief our font
  ngl::Text *m_text;//,*m_text1;

  /// @brief the texture id for our texture
  GLuint m_crowdTextureName,m_tableTextureName,m_speedLegend,m_rpmLegend,m_trajectoryLegend,m_gridXYHtMap;
  GLuint m_heightMapLegend;

  GLuint m_fboID;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief our view matrix
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_view;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief our projection matrix
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_projection;

  int m_legendWidth,m_legendHeight;

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
  /// be re-drawn
  void paintGL();

private :


  void drawEntireScene();

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called every time a mouse is moved
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mouseMoveEvent (
                        QMouseEvent * _event
                      );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is pressed
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mousePressEvent (
                          QMouseEvent *_event
                       );

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is released
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mouseReleaseEvent (
                          QMouseEvent *_event
                          );

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse wheel is moved
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void wheelEvent(
                    QWheelEvent *_event
                 );
  /// @brief the timer event triggered from the timers
  /// @param _even the event of the timer triggered by Qt
  void timerEvent(
                  QTimerEvent *_event
                 );

  void loadMatricesToADSShader();

  void loadMatricesToTextureShader();

  void loadMatricesToLegendShader();

  void fillAllImpactPointsBothPlayers();
  void buildQuadSpecificData();

  void buildPointGraphBars();
  void buildTrajectory();
  void buildSpeedMap();
  void buildRPMData();
  void calculateFieldOfPlayInWorldCoords();

  void setColorBasedOnSpeedRange(float _inputVelocity);
  void setColorBasedOnRPMRange(float _inputRPM);

  void fillDataVertsWithNormals(std::vector<cv::Point3f>& _inputVertList,
                                std::vector<VertData>& o_normalFilledDataVerts);

  void fillDataFromFileForBothPlayers(cv::FileStorage& _fs,WHOSE_STATS _whichPlayer);

  void drawCrowd(const std::string& _inputPlaneName);
  void drawTable(const std::string& _inputTableName);

  void renderPercentages();
  void legendWindow(GLuint _inputLegendName);

  void drawContainingGridScaleXY();

public slots:
  void resetPoints();

  void setWhichStatToView(int _inputWhichStat);

  void setWhoseStatToView(int _inputWhoseStat);

  void setWhichQuadToView(int _inputWhichQuad);

  void setKinectToPlayAreaDistance(double _inputDistanceInMeters);

  void setDiameterOfBallInMeters(double _inputDiaInMeters);

  // file reading and writing trial

  void readFile(QString _inputFileName);
  void writeFile();



//public:
  //void write(cv::FileStorage& fs) const;                       //Write serialization for this class

  //void read(const cv::FileNode& node);                          //Read serialization for this class

};


// the write and read signatures are very important
// this is the rule implied by opencv for serialization

//These write and read functions must be defined for the serialization in FileStorage to work
//static void write(cv::FileStorage& fs, const std::string&, const StatsVisualization& x)
//{
//    x.write(fs);
//}
//static void read(const cv::FileNode& node, StatsVisualization& x, const StatsVisualization& default_value = StatsVisualization())
//{
//// removed the check for validity of the requested node...
//    // have to place it if necessary
//    x.read(node);
//}

////// This function will print our custom class to the console
//static std::ostream& operator<<(std::ostream& out, const StatsVisualization& m)
//{
////    out << "{ id = " << m.id << ", ";
////    out << "X = " << m.X << ", ";
////    out << "A = " << m.A << "}";
//    return out;
//}

#endif // VISUALIZATIONWINDOW_H
