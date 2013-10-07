#ifndef TWODSTATSGENERATION_H
#define TWODSTATSGENERATION_H

#include "opencv2/opencv.hpp"

/// @file StatsVisualization.h
/// @brief stats visualization
/// @author Santosh
/// @version 1.0
/// @date 16/08/13
/// @class StatsVisualization
/// @brief complete stats vz window

class TwoDStatsGeneration
{
    public:
        TwoDStatsGeneration();
        ~TwoDStatsGeneration();

        // stats generatiing functions

        // calculates the actual player impact points from the available data and the indices
        // this will be called by the player class whenever it needs data into impact points
        // vector
        void calculatePlayerImpactPtsFromIndices(std::vector<std::vector<cv::Point3f> > &_inputPlayerBallPts,
                                                 std::vector<std::vector<int> > &_playerDeepestPtIndices,
                                                 std::vector<cv::Point> &o_playerImpactPtsAllRallies
                                                 );


        // given sets of dimensions of the field of play,
        // this would fill up a specific percentage value representing the
        // the corresponding field of play percentage
        // so one set of dimension, returning one percentage value
        void generatePercentageDistribution(int _widthBounds, int _heightBounds,
                                            std::vector<cv::Point> &_playerImpactPtsAllRallies,
                                            int _whichQuadrant,
                                            std::vector<cv::Point>& o_specificQuadPts,
                                            float& o_percentage);






    private:









};
#endif // TWODSTATSGENERATION_H
