// filling up the arrays(setters)
// getting those arrays(getters)

#include "PlayerData.h"

PlayerData::PlayerData()
{
    m_playerBallPts.clear();
    m_playerDeepestPtIndices.clear();
    m_playerImpactPtsAllRallies.clear();
    m_playerImpactPtsAllRalliesIn3D.clear();
    m_playerImpactPtsBottomLeft.clear();
    m_playerImpactPtsTopLeft.clear();
    m_playerImpactPtsTopRight.clear();
    m_playerImpactPtsBottomRight.clear();
    m_barGraphVertices.clear();
    m_playerInterpolatedBallPts.clear();
    m_speedData.clear();

    m_dataProcessed = false;

    m_bottomLeftPercentage = m_topLeftPercentage = 0;
    m_topRightPercentage = m_bottomRightPercentage = 0;

    m_rpmData.clear();

    m_twoDStatsGenerator = new TwoDStatsGeneration();
    m_threeDStatsGenerator = new ThreeDStatsGeneration();
}

PlayerData::~PlayerData()
{
    m_playerBallPts.clear();
    m_playerDeepestPtIndices.clear();
    m_playerImpactPtsAllRallies.clear();
    m_playerImpactPtsAllRalliesIn3D.clear();
    m_playerImpactPtsBottomLeft.clear();
    m_playerImpactPtsTopLeft.clear();
    m_playerImpactPtsTopRight.clear();
    m_playerImpactPtsBottomRight.clear();
    m_barGraphVertices.clear();
    m_playerInterpolatedBallPts.clear();
    m_speedData.clear();
    m_rpmData.clear();

    m_dataProcessed = false;

    delete m_twoDStatsGenerator;
    delete m_threeDStatsGenerator;
}

void PlayerData::setPlayerBallPts(std::vector<std::vector<cv::Point3f> > &_inputPointSetsInWorld)
{
    m_playerBallPts = _inputPointSetsInWorld;
}

void PlayerData::setPlayerImpactPtIndices(std::vector<std::vector<int> > &_inputDepthIndices)
{
    m_playerDeepestPtIndices = _inputDepthIndices;
}

std::vector<std::vector<cv::Point3f> >& PlayerData::getPlayerBallPtsAll()
{
    return m_playerBallPts;
}

std::vector<std::vector<int> >& PlayerData::getPlayerImpactPtIndices()
{
    return m_playerDeepestPtIndices;
}

std::vector<cv::Point>& PlayerData::getPlayerImpactPtsAllRallies()
{
    // we first check if we have already calculated impact points, if so return,
    // else calculate it now
    if(m_playerImpactPtsAllRallies.size() != 0)
    {
        return m_playerImpactPtsAllRallies;
    }
    else
    {
        //calculatePlayerImpactPtsFromIndices();

        m_twoDStatsGenerator->calculatePlayerImpactPtsFromIndices(m_playerBallPts,m_playerDeepestPtIndices,
                                                                  m_playerImpactPtsAllRallies);
        return m_playerImpactPtsAllRallies;
    }
}


// which percentage requested enum value,
// accordingly compute the required dimensions and send it to the 2dstats generator function
// we should first check if rallyDepthPtsAll has been already filled,
// if not fill them first
// this is so that if later someone chose not to show
// impact pts as a defualt statistic by changing the constructor,
// the other stats would still work independetly

float PlayerData::getPercentageDistributionData(int _width, int _height, WHICH_QUADRANT _whichQuadrant)
{

    float tempPercentage = 0.0;

    // call calculate if rally points empty

    if(m_playerImpactPtsAllRallies.size() == 0)
    {

        //calculatePlayerImpactPtsFromIndices();

        m_twoDStatsGenerator->calculatePlayerImpactPtsFromIndices(m_playerBallPts,m_playerDeepestPtIndices,
                                                                  m_playerImpactPtsAllRallies);
    }

    switch(_whichQuadrant)
    {
        case BOTTOM_LEFT:
        {
            m_twoDStatsGenerator->generatePercentageDistribution(_width,_height,m_playerImpactPtsAllRallies,BOTTOM_LEFT,
                                                                 m_playerImpactPtsBottomLeft,m_bottomLeftPercentage);
            tempPercentage = m_bottomLeftPercentage;
            break;
        }
        case TOP_LEFT:
        {
            m_twoDStatsGenerator->generatePercentageDistribution(_width,_height,m_playerImpactPtsAllRallies,TOP_LEFT,
                                                                m_playerImpactPtsTopLeft,m_topLeftPercentage);
            tempPercentage = m_topLeftPercentage;
            break;
        }
        case TOP_RIGHT:
        {
            m_twoDStatsGenerator->generatePercentageDistribution(_width,_height,m_playerImpactPtsAllRallies,TOP_RIGHT,
                                                                m_playerImpactPtsTopRight,m_topRightPercentage);
            tempPercentage = m_topRightPercentage;
            break;
        }
        case BOTTOM_RIGHT:
        {
            m_twoDStatsGenerator->generatePercentageDistribution(_width,_height,m_playerImpactPtsAllRallies,BOTTOM_RIGHT,
                                                                m_playerImpactPtsBottomRight,m_bottomRightPercentage);
            tempPercentage = m_bottomRightPercentage;
            break;
        }
        case ALL:
        {
            // in case data is needed for all quads, caller has to call this function 4 times
            // and save the returning floats separately
            break;
        }
    }

    return tempPercentage;
}

std::vector<cv::Point>& PlayerData::getQuadSpecificPoints(WHICH_QUADRANT _whichQuad)
{

    if(_whichQuad == BOTTOM_LEFT)
    {
        return m_playerImpactPtsBottomLeft;
    }
    else if(_whichQuad == TOP_LEFT)
    {
        return m_playerImpactPtsTopLeft;
    }
    else if(_whichQuad == TOP_RIGHT)
    {
        return m_playerImpactPtsTopRight;
    }
    else if(_whichQuad == BOTTOM_RIGHT)
    {
        return m_playerImpactPtsBottomRight;
    }
}

std::vector<cv::Point3f>& PlayerData::getBarGraphVertices(cv::Point _inputTopXY, int _width, int _height, int _gridResolution)
{

    if(m_barGraphVertices.size() != 0)
    {
        return m_barGraphVertices;
    }

    else
    {
        // call calculate if rally points empty
        if(m_playerImpactPtsAllRallies.size() == 0)
        {

            //calculatePlayerImpactPtsFromIndices();

            m_twoDStatsGenerator->calculatePlayerImpactPtsFromIndices(m_playerBallPts,m_playerDeepestPtIndices,
                                                                      m_playerImpactPtsAllRallies);
        }

        m_threeDStatsGenerator->generatePitchDensityGraphData(_inputTopXY,_width,_height,_gridResolution,m_playerImpactPtsAllRallies,
                                                              m_barGraphVertices);

        return m_barGraphVertices;
    }
}


std::vector<std::vector<cv::Point3f> >& PlayerData::getPlayerInterpolatedBallPtsAll(float _inputDistanceFrmKinectToPlayArea)
{
    if(m_playerInterpolatedBallPts.size() != 0)
    {
        return m_playerInterpolatedBallPts;
    }
    else
    {
        m_threeDStatsGenerator->generateInterpolatedDataForAllRallies(m_playerBallPts,m_playerDeepestPtIndices,
                                                                      _inputDistanceFrmKinectToPlayArea,
                                                                      m_playerInterpolatedBallPts,m_speedData);

       // m_threeDStatsGenerator->generateInterpolatedDataForAllRalliesWithoutImpact(m_playerBallPts,m_playerInterpolatedBallPts);

        return m_playerInterpolatedBallPts;
    }
}


std::vector<float>& PlayerData::getPlayerSpeedData(float _inputDistanceFrmKinectToPlayArea)
{
    // this will be available only when interpolatedballpts function is called
    // as that is where speed data is filled

    //so if interpolatedballpts vec is empty, call the getinterpballpts function
    // and then check if speed data not empty and then return the speeddata vec
    if(m_playerInterpolatedBallPts.size() == 0)
    {
        m_threeDStatsGenerator->generateInterpolatedDataForAllRallies(m_playerBallPts,m_playerDeepestPtIndices,
                                                                      _inputDistanceFrmKinectToPlayArea,
                                                                      m_playerInterpolatedBallPts,m_speedData);
    }
    //if(m_speedData.size() != 0)
    {
        return m_speedData;
    }
}

std::vector<cv::Point3f>& PlayerData::getPlayerImpactPtsIn3DAllRallies()
{
    if(m_playerImpactPtsAllRalliesIn3D.size() != 0)
    {
        return m_playerImpactPtsAllRalliesIn3D;
    }
    else
    {
        m_threeDStatsGenerator->calculatePlayerImpactPtsIn3DFromIndices(m_playerBallPts,m_playerDeepestPtIndices,
                                                                        m_playerImpactPtsAllRalliesIn3D);
        return m_playerImpactPtsAllRalliesIn3D;
    }
}

std::vector<float>& PlayerData::getRevolutionsPerMinute(float _inputDiameterInMeters,
                                                        float _inputDistanceFrmKinectToPlayArea)
{

    // this make sures speed data is generated first
    // which would then feed into rpm calculations
    if(m_playerInterpolatedBallPts.size() == 0)
    {
        m_threeDStatsGenerator->generateInterpolatedDataForAllRallies(m_playerBallPts,m_playerDeepestPtIndices,
                                                                      _inputDistanceFrmKinectToPlayArea,
                                                                      m_playerInterpolatedBallPts,m_speedData);
    }

    if(m_rpmData.size() != 0)
    {
       return m_rpmData;
    }

    else
    {
        for(int i=0;i<m_speedData.size();i++)
        {
            // this produces revolutions per minute
            // input diameter and speed data are in meters scale
            float tempRPM = (m_speedData[i] * 60)/(3.14 * _inputDiameterInMeters);
            std::cout<<"repm calculated:"<<tempRPM<<"\n";
            m_rpmData.push_back(tempRPM);
        }

        return m_rpmData;
    }

}

void PlayerData::setPlayerDataProcessedFlag()
{
    m_dataProcessed = true;
}

bool PlayerData::checkDataProcessed()
{
    return m_dataProcessed;
}
