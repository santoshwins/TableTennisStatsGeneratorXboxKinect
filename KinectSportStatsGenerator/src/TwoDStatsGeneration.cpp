#include <TwoDStatsGeneration.h>

TwoDStatsGeneration::TwoDStatsGeneration()
{

}

TwoDStatsGeneration::~TwoDStatsGeneration()
{

}

void TwoDStatsGeneration::calculatePlayerImpactPtsFromIndices(std::vector<std::vector<cv::Point3f> > &_inputPlayerBallPts,
                                                              std::vector<std::vector<int> > &_playerDeepestPtIndices,
                                                              std::vector<cv::Point> &o_playerImpactPtsAllRallies)
{
    if(_playerDeepestPtIndices.size() != 0)
    {
        for(int i = 0;i<_playerDeepestPtIndices.size();i++)
        {
            for(int j=0;j<_playerDeepestPtIndices[i].size();j++)
            {

                // if the impact point index is not -1, then there is an impact point for this rally
                if(_playerDeepestPtIndices[i][j] != -1)
                {
                    o_playerImpactPtsAllRallies.push_back(cv::Point((int)(_inputPlayerBallPts[i][_playerDeepestPtIndices[i][j]]).x,(int)(_inputPlayerBallPts[i][_playerDeepestPtIndices[i][j]]).y));
                }
                else
                {
                    std::cout<<"No impact point for Rally number:"<<i<<"..so skipping to next rally.\n";
                }

            }
        }
    }
    else
    {
        std::cout<<"No data in indices array for the player.\n";
    }
}



void TwoDStatsGeneration::generatePercentageDistribution(int _widthBounds, int _heightBounds,
                                                         std::vector<cv::Point> &_playerImpactPtsAllRallies,
                                                         int _whichQuadrant,
                                                         std::vector<cv::Point>& o_specificQuadPts,
                                                         float& o_percentage)
{

    switch(_whichQuadrant)
    {
        //bottom left
        case 0:
        {
            for(int i=0;i<_playerImpactPtsAllRallies.size();i++)
            {
                if((_playerImpactPtsAllRallies[i].x <= _widthBounds) && (_playerImpactPtsAllRallies[i].y <= _heightBounds))
                {
                    // if it falls into the requested quadrant then fill in the buffer
                    o_specificQuadPts.push_back(_playerImpactPtsAllRallies[i]);
                }
            }
            break;
        }
        // top left
        case 1:
        {
            for(int i=0;i<_playerImpactPtsAllRallies.size();i++)
            {
                if((_playerImpactPtsAllRallies[i].x <= _widthBounds) && (_playerImpactPtsAllRallies[i].y >= _heightBounds))
                {
                    // if it falls into the requested quadrant then fill in the buffer
                    o_specificQuadPts.push_back(_playerImpactPtsAllRallies[i]);
                }
            }
            break;

        }
        // top right
        case 2:
        {
            for(int i=0;i<_playerImpactPtsAllRallies.size();i++)
            {
                if((_playerImpactPtsAllRallies[i].x > _widthBounds) && (_playerImpactPtsAllRallies[i].y > _heightBounds))
                {
                    // if it falls into the requested quadrant then fill in the buffer
                    o_specificQuadPts.push_back(_playerImpactPtsAllRallies[i]);
                }
            }
        break;

        }
        // bottom right
        case 3:
        {
            for(int i=0;i<_playerImpactPtsAllRallies.size();i++)
            {
                if((_playerImpactPtsAllRallies[i].x > _widthBounds) && (_playerImpactPtsAllRallies[i].y < _heightBounds))
                {
                    // if it falls into the requested quadrant then fill in the buffer
                    o_specificQuadPts.push_back(_playerImpactPtsAllRallies[i]);
                }
            }
            break;

        }
    }


    o_percentage = (float)((float)o_specificQuadPts.size()/(float)_playerImpactPtsAllRallies.size());
    o_percentage = o_percentage * 100;

}





// may be we can have an overloaded function for the above one
// where instead of 4 output buffers,
// we need to generate output into as many buffers as there are grid boxes
// for percentage density


