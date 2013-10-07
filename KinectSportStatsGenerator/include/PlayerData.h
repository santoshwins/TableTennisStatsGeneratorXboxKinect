#ifndef PLAYERDATA_H
#define PLAYERDATA_H

#include "TwoDStatsGeneration.h"
#include "ThreeDStatsGeneration.h"

#include "opencv2/opencv.hpp"

/// @file PlayerData.h
/// @brief data class for players
/// @author Santosh
/// @version 1.0
/// @date 16/08/13
/// @class PlayerData
/// @brief contains all statistics mapped to players

typedef enum
{
    TOP_LEFT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    TOP_RIGHT,
    ALL
}WHICH_QUADRANT;

class PlayerData
{
    public:
        PlayerData();
        ~PlayerData();

        void setPlayerBallPts(std::vector<std::vector<cv::Point3f> > &_inputPointSetsInWorld);
        void setPlayerImpactPtIndices(std::vector<std::vector<int> > &_inputDepthIndices);
        void setPlayerDataProcessedFlag();

        std::vector<cv::Point>& getPlayerImpactPtsAllRallies();

        std::vector<std::vector<cv::Point3f> >& getPlayerBallPtsAll();

        std::vector<std::vector<int> >& getPlayerImpactPtIndices();

        float getPercentageDistributionData(int _width, int _height,WHICH_QUADRANT _whichQuadrant);

        std::vector<cv::Point>& getQuadSpecificPoints(WHICH_QUADRANT _whichQuad);

        std::vector<cv::Point3f>& getBarGraphVertices(cv::Point _inputTopXY, int _width, int _height, int _gridResolution);

        std::vector<std::vector<cv::Point3f> >& getPlayerInterpolatedBallPtsAll(float _inputDistanceFrmKinectToPlayArea);

        std::vector<cv::Point3f>& getPlayerImpactPtsIn3DAllRallies();

        std::vector<float>& getPlayerSpeedData(float _inputDistanceFrmKinectToPlayArea);

        std::vector<float>& getRevolutionsPerMinute(float _inputDiameterInMeters,
                                                    float _inputDistanceFrmKinectToPlayArea);


        bool checkDataProcessed();


    private:

        TwoDStatsGeneration *m_twoDStatsGenerator;
        ThreeDStatsGeneration *m_threeDStatsGenerator;



        // data processed flag
        // checked by the stats window often to see if data has been processed by
        // the procecssing utility so that stats window can then query player's
        // impact points and draw it
        // this flag will be set by the processing utility once data is being filled into each player's
        // vector attirbutes ballpts and deepestptindices
        bool m_dataProcessed;

        // a multidimensional vector to store sets of balls points against sets of rallies
        std::vector<std::vector<cv::Point3f> > m_playerBallPts;

        // a multidimensional vector to store sets of deepest point/impact point indices
        // against sets of corresponding rallies
        std::vector<std::vector<int> > m_playerDeepestPtIndices;


        std::vector<cv::Point> m_playerImpactPtsAllRallies;
        std::vector<cv::Point3f> m_playerImpactPtsAllRalliesIn3D;

        // percentage distribution data from bottom left clockwise
        float m_bottomLeftPercentage,m_topLeftPercentage,m_topRightPercentage;
        float m_bottomRightPercentage;

        // other stats vectors

        //  quad specific data points
        std::vector<cv::Point> m_playerImpactPtsBottomLeft,m_playerImpactPtsTopLeft;
        std::vector<cv::Point> m_playerImpactPtsTopRight,m_playerImpactPtsBottomRight;


        std::vector<cv::Point3f> m_barGraphVertices;


        // a multidimensional vector to store sets of INTERPOLATED BALL
        // POINTS against rallies...as this is interpolated data
        // it would be in ascending order within specific sets
        // sets here means, we classify each set based on the impact
        // points
        std::vector<std::vector<cv::Point3f> > m_playerInterpolatedBallPts;

        // a multidimensional vector holding the velocity data
        // at the impact points
        // against each rally
        std::vector<float> m_speedData;

        std::vector<float> m_rpmData;

        // calculates the actual player impact points from the available data and the indices
        // this will be called by all the stats modules when needed
        void calculatePlayerImpactPtsFromIndices();





};

#endif // PLAYERDATA_H
