#include "ThreeDStatsGeneration.h"

//#include <Spline.hpp>

// to serve as comparison function object for vector SORT
bool compare3FOnX(cv::Point3f i, cv::Point3f j){ return (i.x < j.x);}

ThreeDStatsGeneration::ThreeDStatsGeneration()
{
    m_quadFitX = new QuadCurveFitUtility();
    m_quadFitY = new QuadCurveFitUtility();
    m_quadFitZ = new QuadCurveFitUtility();
}

ThreeDStatsGeneration::~ThreeDStatsGeneration()
{
    delete m_quadFitX;
    delete m_quadFitY;
    delete m_quadFitZ;
}


void ThreeDStatsGeneration::generatePitchDensityGraphData(cv::Point& _userDefinedTopXY, int _userDefinedWidth, int _userDefinedHeight,
                                                        int _gridResolution,
                                                        std::vector<cv::Point> &_playerImpactPtsAllRallies,
                                                        std::vector<cv::Point3f>& o_verticesSetOfCubeBarsGraph)
{
    //for each point in the input array

    // each cell width = inputwidth/gridresolution
    // each cell height = inputheight/gridresolution
    // hashx = floor((point.x - _userDefinedTopXY.x)/each cell width)
    // hashy = floor((point.y - _userDefinedTopXY.y)/each cell height)

    // 1D hash value is hashx + hashy * gridresolution

    // push the point as vec[hashvalue].push_back(point)

    std::vector<std::vector<cv::Point> > hashMappedData;

    // remember to mention these scale factors
    float _scaleFactor = 10.0;
    int _heightUpScaleFactor = 1.0;


    float _scaledWidth = ((float)_userDefinedWidth/_scaleFactor);
    float _scaledHeight = ((float)_userDefinedHeight/_scaleFactor);

    // we scale down by 10 as pixel coords will be too large


    float _scaledTopX = ((float)_userDefinedTopXY.x/_scaleFactor);
    float _scaledTopY = ((float)_userDefinedTopXY.y/_scaleFactor);

    float _individualCellWidth = _scaledWidth/(float)_gridResolution; // typecast fully if problem arises
    float _individualCellHeight = _scaledHeight/(float)_gridResolution;

    int hashIncrement = 0;

    hashMappedData.resize(_gridResolution * _gridResolution);

    hashMappedData.clear();

//    // allocate enough data for hashmap
    for(int i=0;i<_gridResolution * _gridResolution;i++)
    {
        hashMappedData.push_back(std::vector<cv::Point> ());

        hashMappedData[i].push_back(cv::Point(0,0));
    }



    for(int i=0;i<_playerImpactPtsAllRallies.size();i++)
    {
        int hashX = floor((((float)_playerImpactPtsAllRallies[i].x/_scaleFactor) - _scaledTopX)/_individualCellWidth);
        int hashY = floor((((float)_playerImpactPtsAllRallies[i].y/_scaleFactor) - _scaledTopY)/_individualCellHeight);

        int hashValue = (hashX + (hashY * _gridResolution));

        hashMappedData[hashValue].push_back(_playerImpactPtsAllRallies[i]);
    }


    // and then make the grid flow from -w to +w
    // and -z to +z with grid origin at center
    // so subtract w/2 and h/2 respectively

    //float _scaledTopXTranslated = _scaledTopX - (_scaledWidth/2);
    //float _scaledTopYTranslated = _scaledTopY - (_scaledHeight/2);


    float _scaledTopXTranslated =  -(_scaledWidth/2);
    float _scaledTopYTranslated =  -(_scaledHeight/2);

//    float tempBottomX4 = 0.0, tempBottomZ4 = 0.0, tempBottomY4 = 0.0 , tempBottomX3 = 0.0, tempBottomZ3 = 0.0,tempBottomY3 = 0.0;

//    float tempBottomX2 = 0.0, tempBottomZ2 = 0.0,tempBottomY2 = 0.0,tempBottomX1 = 0.0,tempBottomZ1 = 0.0,tempBottomY1 = 0.0;

//    float tempTopX4 = 0.0,tempTopZ4 = 0.0,tempTopY4 = 0.0,tempTopX3 = 0.0,tempTopZ3 = 0.0,tempTopY3 = 0.0,tempTopX2 = 0.0,tempTopZ2 = 0.0;
//    float tempTopY2 = 0.0,tempTopX1 = 0.0,tempTopZ1 = 0.0,tempTopY1 = 0.0;


    //for(float i=_scaledTopYTranslated;i<(_scaledTopYTranslated + _scaledHeight);i=i+_individualCellHeight)
    //{
      //  for(float j=_scaledTopXTranslated;j<(_scaledTopXTranslated + _scaledWidth);j=j+_individualCellWidth)
        //{

    float xPos = _scaledTopXTranslated;
    float zPos = _scaledTopYTranslated;

            for(int i = 0;i<_gridResolution;i++)
            {
                for(int j=0;j<_gridResolution;j++)
                {


                    if((hashMappedData[hashIncrement].size() - 1) == 0)
                    {
                        // no need to push the data as this quad should not be rendered
                        // with 0 height
                        hashIncrement++;

                        xPos = xPos + _individualCellWidth;
                        continue;

                    }
             float tempBottomX4 = xPos;
             float tempBottomZ4 = zPos;
             float tempBottomY4 = 0.0;

             float tempBottomX3 = xPos + _individualCellWidth;
             float tempBottomZ3 = zPos;
             float tempBottomY3 = 0.0;

             float tempBottomX2 = xPos + _individualCellWidth;
             float tempBottomZ2 = zPos + _individualCellHeight;
             float tempBottomY2 = 0.0;

             float tempBottomX1 = xPos;
             float tempBottomZ1 = zPos + _individualCellHeight;
             float tempBottomY1 = 0.0;

             float tempTopX4 = tempBottomX4;
             float tempTopZ4 = tempBottomZ4;
             float tempTopY4 = (hashMappedData[hashIncrement].size() - 1) * _heightUpScaleFactor;

             float tempTopX3 = tempBottomX3;
             float tempTopZ3 = tempBottomZ3;
             float tempTopY3 = (hashMappedData[hashIncrement].size() - 1) * _heightUpScaleFactor;

             float tempTopX2 = tempBottomX2;
             float tempTopZ2 = tempBottomZ2;
             float tempTopY2 = (hashMappedData[hashIncrement].size() - 1) * _heightUpScaleFactor;

             float tempTopX1 = tempBottomX1;
             float tempTopZ1 = tempBottomZ1;
             float tempTopY1 = (hashMappedData[hashIncrement].size() - 1) * _heightUpScaleFactor;


                     //std::cout<<"tempTopX1:"<<tempTop

            if((hashMappedData[hashIncrement].size() - 1) != 0)
            {
                std::cout<<"hashmap Y:"<<(hashMappedData[hashIncrement].size() - 1)<<"\n";
            }

            // top first - clockwise
            o_verticesSetOfCubeBarsGraph.push_back(cv::Point3f(tempTopX1,tempTopY1,tempTopZ1));
            o_verticesSetOfCubeBarsGraph.push_back(cv::Point3f(tempTopX2,tempTopY2,tempTopZ2));
            o_verticesSetOfCubeBarsGraph.push_back(cv::Point3f(tempTopX3,tempTopY3,tempTopZ3));
            o_verticesSetOfCubeBarsGraph.push_back(cv::Point3f(tempTopX4,tempTopY4,tempTopZ4));

            // bottom next - clockwise
            o_verticesSetOfCubeBarsGraph.push_back(cv::Point3f(tempBottomX1,tempBottomY1,tempBottomZ1));
            o_verticesSetOfCubeBarsGraph.push_back(cv::Point3f(tempBottomX2,tempBottomY2,tempBottomZ2));
            o_verticesSetOfCubeBarsGraph.push_back(cv::Point3f(tempBottomX3,tempBottomY3,tempBottomZ3));
            o_verticesSetOfCubeBarsGraph.push_back(cv::Point3f(tempBottomX4,tempBottomY4,tempBottomZ4));

            // incerement the hash index as we move on to the next row
            hashIncrement++;

            xPos = xPos + _individualCellWidth;

        }

                xPos = _scaledTopXTranslated;
                zPos = zPos + _individualCellHeight;


    }

}


void ThreeDStatsGeneration::generateInterpolatedDataForAllRallies(std::vector<std::vector<cv::Point3f> > &_inputPlayerBallPts,
                                                                  std::vector<std::vector<int> > &_playerDeepestPtIndices,
                                                                  float _inputDistanceFrmKinectToPlayArea,
                                                                  std::vector<std::vector<cv::Point3f> > &o_interpPlayerBallPts,
                                                                  std::vector<float> &o_speedData)
{

    int tempFirstIndex = 0;
    int tempLastIndex = 0;

    float paramT = 0.0;

    float deltaT = 0.0;

    // coeffs for velocity calculation
    float coeffX,coeffY,coeffZ;

    coeffX = coeffY = coeffZ = 0.0;

    float tempVelocity = 0.0;
    float tempCoeffSquared = 0.0;

  /*  magnet::math::Spline splineX,splineY,splineZ;

    splineX.clear();
    splineY.clear();
    splineZ.clear();*/


    // for each rally
    for(int i=0;i<_inputPlayerBallPts.size();i++)
    {


        // if there is no data in the depth point for this rally,
        // then we have come to the end dummy space which we
        // pushed in for rally direction change in ballprocessing
        if(_playerDeepestPtIndices[i].size() == 0)
        {
            continue;
        }
        // allocate data for our grand big interpolated data
        // for this rally
        o_interpPlayerBallPts.push_back(std::vector<cv::Point3f>());
        o_interpPlayerBallPts[i].clear();

        // allocate data correspondingly for our velocity
        // for the current rally impact points
        //o_speedData.push_back(std::vector<float> ());
        //o_speedData[i].clear();

        // for each depth point in the current rally,
        // extract from rally[j] to rally[j+1]
        // execute this for depthsize + 1 as we need to extract from one start depth
        // to another depth..so we need j and j+1 always
        // we will understand when we write the code////note to self: fill this comment
        for(int j=0;j<(_playerDeepestPtIndices[i].size() + 1);j++)
        {
            // when j == size - 1
            // then we need to extract the rest from _inputpoints[j] until _inputpoints[_inputpoints.size - 1]
            //tempFirstIndex = j;

            if(j == 0)
            {
                tempFirstIndex = j;
                if(_playerDeepestPtIndices[i][j] != -1)
                {
                    tempLastIndex = _playerDeepestPtIndices[i][j];
                }
                // if -1, then no impact data detected for this rally, so extract from first to last of the rally
                // nothing in intermediate
                else
                {
                    tempLastIndex = (_inputPlayerBallPts[i].size() - 1);
                }
            }
            else if (j == _playerDeepestPtIndices[i].size())
            {
                tempFirstIndex = _playerDeepestPtIndices[i][(j-1)];
                tempLastIndex = (_inputPlayerBallPts[i].size() - 1);
            }
            else
            {
                tempFirstIndex = _playerDeepestPtIndices[i][(j-1)];
                tempLastIndex = _playerDeepestPtIndices[i][j];
            }


            std::vector<cv::Point3f>::const_iterator first = _inputPlayerBallPts[i].begin() + tempFirstIndex;
            std::vector<cv::Point3f>::const_iterator last = _inputPlayerBallPts[i].begin() + tempLastIndex + 1;

            std::vector<cv::Point3f> tempExtractedPts(first,last);

            // sort this temp vector of 3fs
            std::sort(tempExtractedPts.begin(),tempExtractedPts.end(),compare3FOnX); // need to check if this really compares

            std::vector<cv::Point3f> tempExtractedPtsInWorldCoords;
            tempExtractedPtsInWorldCoords.resize(tempExtractedPts.size());
            // convert to world coords before interpolating
            convertToWorldFromScreen(tempExtractedPts,tempExtractedPtsInWorldCoords, _inputDistanceFrmKinectToPlayArea);


            // 3 buffers for interpX,interpY, interpZ
            std::vector<float> _bufferX,_bufferY,_bufferZ;

            _bufferX.clear();
            _bufferY.clear();
            _bufferZ.clear();

            paramT = 0.0;
            deltaT = 0.0;

            // for each extracted stream different coeffs, so reset to 0
            coeffX = coeffY  = coeffZ = 0.0;

           /* splineX.clear();
            splineY.clear();
            splineZ.clear();*/

            if(tempExtractedPtsInWorldCoords.size() != 0)
            {
                deltaT = (float)(1.0/tempExtractedPtsInWorldCoords.size());
            }

           for(int xIndex =0; xIndex<tempExtractedPtsInWorldCoords.size();xIndex++)
           {


               m_quadFitX->addPoints(cv::Point2f(paramT,tempExtractedPtsInWorldCoords[xIndex].x));

               /*splineX.addPoint(paramT,tempExtractedPtsInWorldCoords[xIndex].x);*/

               paramT += deltaT;


           }

           /*
           //A spline which turns into a parabola at the boundaries.

           //This BC does not need extra parameters, so just the condition is
           //enough.
           splineX.setLowBC(magnet::math::Spline::PARABOLIC_RUNOUT_BC);
           splineX.setHighBC(magnet::math::Spline::PARABOLIC_RUNOUT_BC);

           for (double x = (-0.2); x <= 1.2001; x += 0.005)
           {
              float temp = splineX(x);

             _bufferX.push_back(temp);
           }
            */

           m_quadFitX->processParametricPoints3D(_bufferX,coeffX);


           paramT = 0.0;
           for(int yIndex =0; yIndex<tempExtractedPtsInWorldCoords.size();yIndex++)
           {


               m_quadFitY->addPoints(cv::Point2f(paramT,tempExtractedPtsInWorldCoords[yIndex].y));

               /*splineY.addPoint(paramT,tempExtractedPtsInWorldCoords[yIndex].y);*/

               paramT += deltaT;


           }

           /*
           //A spline which turns into a parabola at the boundaries.

           //This BC does not need extra parameters, so just the condition is
           //enough.
           splineY.setLowBC(magnet::math::Spline::PARABOLIC_RUNOUT_BC);
           splineY.setHighBC(magnet::math::Spline::PARABOLIC_RUNOUT_BC);

           for (double x = (-0.2); x <= 1.2001; x += 0.005)
           {
              float temp = splineY(x);

             _bufferY.push_back(temp);
           }
           */

           m_quadFitY->processParametricPoints3D(_bufferY,coeffY);


           paramT = 0.0;
           for(int zIndex =0; zIndex<tempExtractedPtsInWorldCoords.size();zIndex++)
           {


               m_quadFitZ->addPoints(cv::Point2f(paramT,tempExtractedPtsInWorldCoords[zIndex].z));

               /*splineZ.addPoint(paramT,tempExtractedPtsInWorldCoords[zIndex].z);*/

               paramT += deltaT;


           }

           /*
           //A spline which turns into a parabola at the boundaries.

           //This BC does not need extra parameters, so just the condition is
           //enough.
           splineZ.setLowBC(magnet::math::Spline::PARABOLIC_RUNOUT_BC);
           splineZ.setHighBC(magnet::math::Spline::PARABOLIC_RUNOUT_BC);

           for (double x = (-0.2); x <= 1.2001; x += 0.005)
           {
              float temp = splineZ(x);

             _bufferZ.push_back(temp);
           }
           */

           m_quadFitZ->processParametricPoints3D(_bufferZ,coeffZ);


           if((_bufferX.size() != 0) && (_bufferY.size() != 0) && (_bufferZ.size() != 0)) // checking if we have data in the buffers..need to place additional check to see if all the sizes are equal
           {
               for(int fillIndex =0; fillIndex<_bufferX.size();fillIndex++)
               {


                   std::cout<<"BufferX:"<<_bufferX[fillIndex]<<"BufferY:"<<_bufferY[fillIndex]<<"BufferZ:"<<_bufferZ[fillIndex]<<"\n";

                   // our grand data (for this rally for this extracted chunk) is pushed in
                   o_interpPlayerBallPts[i].push_back(cv::Point3f(_bufferX[fillIndex],_bufferY[fillIndex],_bufferZ[fillIndex]));


               }
           }

           // push speed data for current rally's current extracted stream
           // made it single dimensional as we do not need rally specific impact points
           // so its a straight 1D vector corresponding to impactpoints3d vector
           // filled thru the calculateimpactptin3dfromindices function

           // this check will filter other coeeficients which are actually not at the depth index,
           // because, apart from depth index, we also pass in
           // parts of array from o to depthindex or from deothindex to next depthindex
           // where the intermediate depth index wud push 2 different speeds
           // for the same point
           // so we should avoid pushing speed at those times

           tempCoeffSquared = pow(coeffX,2) + pow(coeffY,2) + pow(coeffZ,2);
           if(tempCoeffSquared <= 0.0)
           {
               tempVelocity = 0.0;
           }
           else
           {
               tempVelocity = sqrt(tempCoeffSquared);
           }

           //std::cout<<"TempVelocity:"<<tempVelocity<<"mpersec\n";

           if(tempLastIndex == _playerDeepestPtIndices[i][j])
           {
                o_speedData.push_back(tempVelocity);
           }

           if(tempLastIndex == (_inputPlayerBallPts[i].size() - 1))
           {
               // we break out of the for loop
               // because if lastindex has a value representing the last element of the
               // rally point, there is nothing more to extract
               // this last point could have been reached in many ways,
               // lik when there is no impact data, 0 to last is extracted
               // or if legitemately we have come until the last index like
               // 0 ---- impactindex1 ------ impactindex2 ----- lastindex
               // or 0 -----impactindex1-----impactindex2(which itself might be referring to the lastindex,
               // of the rally, so we should itself break out instead of extracting forward)
               break;
           }

        }

    }
}


void ThreeDStatsGeneration::convertToWorldFromScreen(std::vector<cv::Point3f>& _inputPointVec, std::vector<cv::Point3f>& o_PointsInWorld, float _inputDistanceFrmKinectToPlayArea)
{
    // THIS IS TAKEN FROM THE OPENCV WIKI PAGE...
    // ALTERNATIVELY, THE TANGENT FORMULAE IN THE IMAGE SAVED IN DESKTOP CAN ALSO BE USED...DO THIS LATER IF ACCURACY SEEMS MISSING
    // or from this blog http://magicalkinect.wordpress.com/2013/02/16/metric-measuring-using-kinect/

//    float minDistance = -10;
//    float scaleFactor = 0.0021;
//    float x = (_iScreen - 640 / 2) * (_zDepth + minDistance) * scaleFactor * (640/480);
//    float y = (_jScreen - 480 / 2) * (_zDepth + minDistance) * scaleFactor;

    for (int i=0;i<_inputPointVec.size();i++)
    {
        float x = (_inputPointVec[i].x/640 - 0.5) * _inputPointVec[i].z * 1.111467f;
        float y = (0.5 - _inputPointVec[i].y/480) * _inputPointVec[i].z * 0.833599f;

        // we can dereference and assign instead of push back because
        // we have resized this vector in the parent function

        // my conversion in the view of the kinect
        //o_PointsInWorld[i] = cv::Point3f(-(y),(1 - _inputPointVec[i].z),-x);

        // this should actually be ditancefromkinecttotable - z as the table is at 0.762
        // so if the ball is at 0.762 from kinect it will be 0 frm the table
        // if its at 0.562 from the kinect, it would be at 0.2 from the table
        o_PointsInWorld[i] = cv::Point3f((x),fabs(_inputDistanceFrmKinectToPlayArea - _inputPointVec[i].z),-(y));
    }

//    float temp = (58.12/2) * (3.14/180);
//    std::cout<<".........."<< (2 * tan(temp));

 //   std::cout<<" X, Y and Z are:"<<x<<"::"<<y<<"::"<<_zDepth<<"\n";



   // m_ballPoints.push_back(cv::Point3f(x,y,(_zDepth)));



}




// rally specific trajectory if needed can be calculated using the rally number input, full ball points,
// rally number should be valid...0 to size - 1 of the ball pts array


//SPEED PITCH MAP

// once we have equation of 3d data for each rally, we can calculate and display the velocity at the impact points
// for each player and use color coding to display the output

// this will convert the XY of impact point into XZ of world coordinates to
// project in opengl
// the resulting vector set would map directly to the 1D speed vector populated
// in the getinterpolateddataforallrallies function
void ThreeDStatsGeneration::calculatePlayerImpactPtsIn3DFromIndices(std::vector<std::vector<cv::Point3f> > &_inputPlayerBallPts,
                                                              std::vector<std::vector<int> > &_playerDeepestPtIndices,
                                                              std::vector<cv::Point3f> &o_playerImpactPtsAllRallies3D)
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



                    float x = (_inputPlayerBallPts[i][_playerDeepestPtIndices[i][j]].x/640 - 0.5) *
                              _inputPlayerBallPts[i][_playerDeepestPtIndices[i][j]].z * 1.111467f;

                    float y = (0.5 - _inputPlayerBallPts[i][_playerDeepestPtIndices[i][j]].y/480) *
                               _inputPlayerBallPts[i][_playerDeepestPtIndices[i][j]].z * 0.833599f;

                    // the plane is going to be at 0,0,0 only
                    // and the Y is going to be 0 as this is going to be a projection of
                    // XY of pixels into XZ in opengl but in world coords as opposed to
                    // pixel coords
                    // remeber this is scaled up to 10
                    o_playerImpactPtsAllRallies3D.push_back(cv::Point3f(10 * x,0.0,-(10 * y)));
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

