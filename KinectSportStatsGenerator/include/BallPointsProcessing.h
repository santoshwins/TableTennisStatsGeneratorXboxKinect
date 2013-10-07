#ifndef BALLPOINTSPROCESSING_H
#define BALLPOINTSPROCESSING_H

#include "PlayerData.h"
#include "opencv2/opencv.hpp"
#include <QObject>

/// @file BallPointsProcessing.h
/// @brief ball track utility
/// @author Santosh
/// @version 1.0
/// @date 16/08/13
/// @class BallPointsProcessing
/// @brief processes the entire track data

class BallPointsProcessing:public QObject
{
    Q_OBJECT // for slots

    private:
        std::vector<cv::Point3f> m_entireTrackedDataPts;
        std::vector<std::vector<cv::Point3f> > m_playerAPts;
        std::vector<std::vector<cv::Point3f> > m_playerBPts;

        std::vector<std::vector<int> > m_playerADeepestPtIndices;
        std::vector<std::vector<int> > m_playerBDeepestPtIndices;

        PlayerData *m_playerA,*m_playerB;

        float m_ballImpactDepthMaxThresh,m_ballImpactDepthMinThresh;

    public:
        BallPointsProcessing();
        BallPointsProcessing(PlayerData *_playerA,PlayerData *_playerB);
        ~BallPointsProcessing();

        // will push data into an vector as and when tracked points arrive
        void setTrackedPoints(cv::Point3f _inputTrackedPoint);

    public slots:
        //when GENERATE STATS button is pressed or when atleast 10 pts have been tracked
        // checks if collected data vector is empty, if not generate player specific data

        void processTrackingData();

        void setBallImpactDepthMaxThreshold(double _impactPtDepthMaxThresh);

        void setBallImpactDepthMinThreshold(double _impactPtDepthMinThresh);


};

#endif // BALLPOINTSPROCESSING_H
