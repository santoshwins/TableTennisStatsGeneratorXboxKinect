#include "BallPointsProcessing.h"
#include <QMessageBox>


BallPointsProcessing::BallPointsProcessing()
{
    m_entireTrackedDataPts.clear();
    m_playerAPts.clear();
    m_playerBPts.clear();
    m_playerADeepestPtIndices.clear();
    m_playerBDeepestPtIndices.clear();

    m_ballImpactDepthMaxThresh = 0.70; // to be decided after measuring with tape
    m_ballImpactDepthMinThresh = 0.60;

}

BallPointsProcessing::BallPointsProcessing(PlayerData *_playerA,PlayerData *_playerB):m_playerA(_playerA),m_playerB(_playerB)
{
    m_entireTrackedDataPts.clear();
    m_playerAPts.clear();
    m_playerBPts.clear();
    m_playerADeepestPtIndices.clear();
    m_playerBDeepestPtIndices.clear();

    m_ballImpactDepthMaxThresh = 0.70; // to be decided after measuring with tape
    m_ballImpactDepthMinThresh = 0.60;

}

BallPointsProcessing::~BallPointsProcessing()
{
    m_entireTrackedDataPts.clear();
    m_playerAPts.clear();
    m_playerBPts.clear();
    m_playerADeepestPtIndices.clear();
    m_playerBDeepestPtIndices.clear();
}

// pushes each tracked point into vector for later processing
void BallPointsProcessing::setTrackedPoints(cv::Point3f _inputTrackedPoint)
{
    m_entireTrackedDataPts.push_back(_inputTrackedPoint);
}


//when GENERATE STATS button is pressed or when atleast 10 pts have been tracked/ or when TRACKING is STOPPED
// checks if collected data vector is empty, if not generate player specific data

// traverses through the entire set of data and on the basis of X component classifies the data as
// player A's or player B's

// data comes in pixel coords...so -ve axis
// simple cmparison enough
// also world coords conversion shud be brought into this class

void BallPointsProcessing::processTrackingData()
{
    int _dataSetSize = m_entireTrackedDataPts.size();

    std::cout<<"DataSize is:"<<_dataSetSize<<"\n";


    int _rallyIncrementIndexLR = 0;
    int _rallyIncrementIndexRL = 0;

    bool _rightToLeft = false;
    bool _leftToRight = false;


    // initial initialization for the first sets of rally....
    m_playerAPts.push_back(std::vector<cv::Point3f> ());
    m_playerBPts.push_back(std::vector<cv::Point3f> ());

    m_playerADeepestPtIndices.push_back(std::vector<int> ());
    m_playerBDeepestPtIndices.push_back(std::vector<int> ());

    if(_dataSetSize != 0)
    {
        for(int i=0;(i+1)<_dataSetSize;i++) // we shud stop one element before the last as there wud be no next elt to compare it with
        {

            // Rally direction classification

            // notes to self:

            // equality condition not needed as 2 pts equal and next to each other is most probably
            // a redundant data rather than actual data as the ball when travelling
            // with high speed is getting detected twice at almost the same locaions
            // so one of the values can be safely ignored
            // so if i == i+1,it would be classified into a single rally once

            if((int)(m_entireTrackedDataPts[i].x) < (int)(m_entireTrackedDataPts[i+1].x)) // changed to <= as we have pasted it inverted
                                                                               // but the data is from 0,0 at the top right
            {
                 //player left to right ( when viwed in perspective of kinet's position)
                // lesser pixels to greater pixels assuming 0,0 at top right of table

                // if previously there was a rally from right to left,
                // then there is a direction change now, hence it has come into this if
                // hence we need to increment rally index and
                // set the current direction to true as long as it comes into this if
                if(_rightToLeft == true)
                {

                    if(m_playerBDeepestPtIndices[_rallyIncrementIndexRL].size() == 0)
                    {
                        m_playerBDeepestPtIndices[_rallyIncrementIndexRL].push_back(-1);

                    }

                    // duplication of end points when direction changes
                    m_playerBPts[_rallyIncrementIndexRL].push_back(m_entireTrackedDataPts[i]);

                    _rallyIncrementIndexRL++;
                    m_playerBPts.push_back(std::vector<cv::Point3f> ());
                    m_playerBDeepestPtIndices.push_back(std::vector<int> ());
                    _rightToLeft = false;


                }




                m_playerAPts[_rallyIncrementIndexLR].push_back(m_entireTrackedDataPts[i]);


                // Impact point classification

                // if fortunately we have actually got a point beyond our impact point thrsh
                // then this must be the impact point
                if(m_entireTrackedDataPts[i].z > m_ballImpactDepthMaxThresh)
                {
                    m_playerADeepestPtIndices[_rallyIncrementIndexLR].push_back((m_playerAPts[_rallyIncrementIndexLR].size() - 1));
                }

                // else we need to check for polarity change
                // if there is a positive polarity(sign) in the difference among the depth values
                // of 3 consecutively detected points then this is an impact point

                // i.e. if sign of the difference in depth values between 1 and 2 is positive and
                // sign of the difference in depth values between 2 and 3 is also positive
                // then the ball must have pitched on the ground
                else
                {
                    // depth detected should atleast be > this min thresh
                    // else some points which has a depth of say 0.5 which is supposed to
                    // be not compared for polarity changes will be taken for comparison
                    // this will be useful when data points are low in number, and
                    // no data is actually valid for depth..this might not be useful
                    // if detection is proper

                    // first point wont fit into this condition as we need 3 points atleast
                    if((i !=0) && (m_entireTrackedDataPts[i].z > m_ballImpactDepthMinThresh) )
                    {
                        if(((m_entireTrackedDataPts[i].z - m_entireTrackedDataPts[i-1].z) > 0) && ((m_entireTrackedDataPts[i].z - m_entireTrackedDataPts[i+1].z) > 0))
                        {
                            // we need to push the index of this point
                            // so push the (current size - 1) of the ith rally which will refer to the current ball point
                            // as vector starts from 0

                            m_playerADeepestPtIndices[_rallyIncrementIndexLR].push_back((m_playerAPts[_rallyIncrementIndexLR].size() - 1));
                        }
                    }
                }

                // if next point is the last point
                // then push the last point as part of the current rally

                if((i+1) == (_dataSetSize -1))
                {
                    //- for Rally Classification

                    //m_playerAPts.push_back(std::vector<cv::Point3f> ());

                    // next elt is the last elt..push it into the same player rally as the prev point
                    m_playerAPts[_rallyIncrementIndexLR].push_back(m_entireTrackedDataPts[i+1]);

                    //- for Impact Point Classification

                    // as this last point may actually be an impact point
                    // we cannot perform the other logic of polarity change, but
                    // we can atleast perform the depth thresh check
                    if(m_entireTrackedDataPts[i+1].z > m_ballImpactDepthMaxThresh)
                    {
                        m_playerADeepestPtIndices[_rallyIncrementIndexLR].push_back((m_playerAPts[_rallyIncrementIndexLR].size() - 1));
                    }


                }

                // if next point is the last point - for Depth classification

                // in case there was no depth change detected until the last point
                // then the last point will be automatically treated as an end point with regards to interpolation
                // and there will be no impact point

                _leftToRight = true;
            }
            else if ((int)(m_entireTrackedDataPts[i].x) > (int)(m_entireTrackedDataPts[i+1].x))
            {
                //  player right to left as seen from kinects top view
                // greater pixels to lesser pixels assuming 640,480 at left bottom of table

                // if previously there was a rally from left to right,
                // then there is a sirection change now, hence it has come into this else
                // hence we need to increment rally index and
                // set the current direction to true as long as it comes into this else

                if(_leftToRight == true)
                {
                    if(m_playerADeepestPtIndices[_rallyIncrementIndexLR].size() == 0)
                    {
                        m_playerADeepestPtIndices[_rallyIncrementIndexLR].push_back(-1);

                    }

                    // duplication of end points when direction changes
                    m_playerAPts[_rallyIncrementIndexLR].push_back(m_entireTrackedDataPts[i]);

                    _rallyIncrementIndexLR++;
                    m_playerAPts.push_back(std::vector<cv::Point3f> ());
                    m_playerADeepestPtIndices.push_back(std::vector<int> ());
                    _leftToRight = false;
                }




                m_playerBPts[_rallyIncrementIndexRL].push_back(m_entireTrackedDataPts[i]);


                // Impact point classification
                // same logic

                if(m_entireTrackedDataPts[i].z > m_ballImpactDepthMaxThresh)
                {
                    m_playerBDeepestPtIndices[_rallyIncrementIndexRL].push_back((m_playerBPts[_rallyIncrementIndexRL].size() - 1));
                }

                else
                {

                    if((i !=0) && (m_entireTrackedDataPts[i].z > m_ballImpactDepthMinThresh) )
                    {
                        if(((m_entireTrackedDataPts[i].z - m_entireTrackedDataPts[i-1].z) > 0) && ((m_entireTrackedDataPts[i].z - m_entireTrackedDataPts[i+1].z) > 0))
                        {
                            m_playerBDeepestPtIndices[_rallyIncrementIndexRL].push_back((m_playerBPts[_rallyIncrementIndexRL].size() - 1));
                        }
                    }
                }

                // if next point is the last point
                // same logic

                if((i+1) == (_dataSetSize -1))
                {

                    //- for Rally Classification

                    //m_playerBPts.push_back(std::vector<cv::Point3f> ());

                    // next elt is the last elt..push it into the same player rally as the prev point
                    m_playerBPts[_rallyIncrementIndexRL].push_back(m_entireTrackedDataPts[i+1]);

                    //- for Impact Point Classification

                    if(m_entireTrackedDataPts[i+1].z > m_ballImpactDepthMaxThresh)
                    {
                        m_playerBDeepestPtIndices[_rallyIncrementIndexRL].push_back((m_playerBPts[_rallyIncrementIndexRL].size() - 1));
                    }
                }

                // if next point is the last point for Depth classification

                // in case there was no depth change detected until the last point
                // then the last point will be automatically treated as an end point with regards to interpolation
                // and there will be no impact point

                _rightToLeft = true;
            }


        }

        // set the playerA vector and playerB vector into the respective instances
        // by calling their set() functions


        m_playerA->setPlayerBallPts(m_playerAPts);
        m_playerA->setPlayerImpactPtIndices(m_playerADeepestPtIndices);

        m_playerB->setPlayerBallPts(m_playerBPts);
        m_playerB->setPlayerImpactPtIndices(m_playerBDeepestPtIndices);


        int aRallies = 0;
        int bRallies = 0;

        for(int i = 0;i<m_playerAPts.size();i++)
        {
            for(int j=0;j<m_playerAPts[i].size();j++)
            {
                if(j==0)
                {
                    aRallies++;
                    std::cout<<"New rally starts....\n";
                }
                std::cout<<"PlayerA points are:"<<m_playerAPts[i][j]<<"\n";
            }
        }

        for(int i = 0;i<m_playerBPts.size();i++)
        {
            for(int j=0;j<m_playerBPts[i].size();j++)
            {
                if(j==0)
                {
                    bRallies++;
                    std::cout<<"New rally starts....\n";
                }
                std::cout<<"PlayerB points are:"<<m_playerBPts[i][j]<<"\n";
            }
        }


        // these are the proper rally counts
        // as we push an empty vector set each time there is a direction change into the
        // opposite direction's vectorofvector for the next anitcipated direction change
        // which may not get filled in as the anticipated next change of direction
        // might not have occured which leaves the last pushed vector set empty
        // hence the rally increment counter has to be incremented only when there is data in the
        // inner vector set..this increment is done in the above inner for loop

        std::cout<<"A rallies:\n"<<aRallies;
        std::cout<<"B rallies\n:"<<bRallies;



        for(int i = 0;i<m_playerADeepestPtIndices.size();i++)
        {
            for(int j=0;j<m_playerADeepestPtIndices[i].size();j++)
            {
                std::cout<<"PlayerA Impact index are:"<<m_playerADeepestPtIndices[i][j]<<"\n";

                std::cout<<"which is"<<m_playerAPts[i][m_playerADeepestPtIndices[i][j]]<<"\n";
            }
        }

        for(int i = 0;i<m_playerBDeepestPtIndices.size();i++)
        {
            for(int j=0;j<m_playerBDeepestPtIndices[i].size();j++)
            {
                std::cout<<"PlayerB Impact index are:"<<m_playerBDeepestPtIndices[i][j]<<"\n";

                std::cout<<"which is"<<m_playerBPts[i][m_playerBDeepestPtIndices[i][j]]<<"\n";
            }
        }


        // set the data processed flag for both the players as we have now filled all the points needed
        // for further stats calculation
        // this flag when set will be queried by the stats window to gather data from the player's
        // object
        m_playerA->setPlayerDataProcessedFlag();
        m_playerB->setPlayerDataProcessedFlag();

    }
    else
    {

        QString warningString("Oops.Enough data points not available. Relax!!! Start Tracking again and Play to generate new data!!!");
        QMessageBox * warning = new QMessageBox(QMessageBox::Warning,QString("NO DATA WARNING"),warningString,QMessageBox::Ok);
        warning->setModal(true);
        warning->setText(warningString);
        warning->show();

        // display this in message box
    }
}

void BallPointsProcessing::setBallImpactDepthMaxThreshold(double _impactPtDepthMaxThresh)
{
    m_ballImpactDepthMaxThresh = (float)_impactPtDepthMaxThresh;
}

void BallPointsProcessing::setBallImpactDepthMinThreshold(double _impactPtDepthMinThresh)
{
    m_ballImpactDepthMinThresh = (float)_impactPtDepthMinThresh;
}


