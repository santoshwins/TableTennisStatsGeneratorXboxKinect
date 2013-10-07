#ifndef THREEDSTATSGENERATION_H
#define THREEDSTATSGENERATION_H

#include "opencv2/opencv.hpp"
#include "QuadCurveFitUtility.h"

/// @file StatsVisualization.h
/// @brief stats visualization
/// @author Santosh
/// @version 1.0
/// @date 16/08/13
/// @class StatsVisualization
/// @brief complete stats vz window

class ThreeDStatsGeneration
{
    public:
        ThreeDStatsGeneration();
        ~ThreeDStatsGeneration();

        void generatePitchDensityGraphData(cv::Point& _userDefinedTopXY, int _userDefinedWidth, int _userDefinedHeight,
                                           int _gridResolution,
                                           std::vector<cv::Point> &_playerImpactPtsAllRallies,
                                           std::vector<cv::Point3f>& o_verticesSetOfCubeBarsGraph);


        void generateInterpolatedDataForAllRallies(std::vector<std::vector<cv::Point3f> > &_inputPlayerBallPts,
                                                   std::vector<std::vector<int> > &_playerDeepestPtIndices,
                                                   float _inputDistanceFrmKinectToPlayArea,
                                                   std::vector<std::vector<cv::Point3f> > &o_interpPlayerBallPts,
                                                   std::vector<float> &o_speedData);

        void calculatePlayerImpactPtsIn3DFromIndices(std::vector<std::vector<cv::Point3f> > &_inputPlayerBallPts,
                                                     std::vector<std::vector<int> > &_playerDeepestPtIndices,
                                                     std::vector<cv::Point3f> &o_playerImpactPtsAllRallies3D);


    private:

        QuadCurveFitUtility *m_quadFitX,*m_quadFitY,*m_quadFitZ;

        void convertToWorldFromScreen(std::vector<cv::Point3f>& _inputPointVec,
                                      std::vector<cv::Point3f>& o_PointsInWorld,
                                      float _inputDistanceFrmKinectToPlayArea);

};

/*

  void BallPointsProcessing::convertToWorldFromScreen(float _iScreen, float _jScreen, float _zDepth)

  */


#endif // 3DSTATSGENERATION_H
