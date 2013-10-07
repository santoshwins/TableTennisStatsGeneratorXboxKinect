/********
  This least squares fitting is partly sources from
  http://www.codeproject.com/Articles/63170/Least-Squares-Regression-for-Quadratic-Curve-Fitti
********/

#include "QuadCurveFitUtility.h"
#include "ngl/Vec3.h"


const int MIN_VALUES = 3;


QuadCurveFitUtility::QuadCurveFitUtility()
{
    m_dataPoints.clear();
    m_dataPointsAfterInterpol.clear();
    m_hermiteInterPolPts.clear();
    m_dataPointsHermite.clear();
    m_a = m_b = m_c = 0;
    m_steps = 0;
    m_noOfPoints = 5;
}

QuadCurveFitUtility::~QuadCurveFitUtility()
{

}
///-------------------------------------------------------------------------------------------------
/// \fn	void CCurveFit::AddPoints( double x, double y )
///
/// \brief	Adds the points to 'y'.
///
/// \param	x	The double to process.
/// \param	y	The double to process.
///-------------------------------------------------------------------------------------------------
void QuadCurveFitUtility::addPoints( cv::Point2f _inputSet )
{
    m_dataPoints.push_back(_inputSet);
}



/*
  y = ax^2 + bx + c

  notation Sjk to mean the sum of x_i^j*y_i^k.
  2a*S40 + 2c*S20 + 2b*S30 - 2*S21 = 0
  2b*S20 + 2a*S30 + 2c*S10 - 2*S11 = 0
  2a*S20 + 2c*S00 + 2b*S10 - 2*S01 = 0

  solve the system of simultaneous equations using Cramer's law

  [ S40  S30  S20 ] [ a ]   [ S21 ]
  [ S30  S20  S10 ] [ b ] = [ S11 ]
  [ S20  S10  S00 ] [ c ]   [ S01 ]

  D = [ S40  S30  S20 ]
      [ S30  S20  S10 ]
      [ S20  S10  S00 ]

      S40(S20*S00 - S10*S10) - S30(S30*S00 - S10*S20) + S20(S30*S10 - S20*S20)

 Da = [ S21  S30  S20 ]
      [ S11  S20  S10 ]
      [ S01  S10  S00 ]

      S21(S20*S00 - S10*S10) - S11(S30*S00 - S10*S20) + S01(S30*S10 - S20*S20)

 Db = [ S40  S21  S20 ]
      [ S30  S11  S10 ]
      [ S20  S01  S00 ]

      S40(S11*S00 - S01*S10) - S30(S21*S00 - S01*S20) + S20(S21*S10 - S11*S20)

 Dc = [ S40  S30  S21 ]
      [ S30  S20  S11 ]
      [ S20  S10  S01 ]

      S40(S20*S01 - S10*S11) - S30(S30*S01 - S10*S21) + S20(S30*S11 - S20*S21)

 */


///-------------------------------------------------------------------------------------------------
/// \fn	double CCurveFit::GetATerm();
///
/// \brief	Gets the a term of the equation ax^2 + bx + c.
///
/// \return	a term.
///-------------------------------------------------------------------------------------------------
double QuadCurveFitUtility::getATerm()
{
    if( m_dataPoints.size() < MIN_VALUES )
    {
        std::cout<<"Atleast 3 data sets required to interpolate" ;
        return 0;
    }

    // notation sjk to mean the sum of x_i^j_i^k.
    double s40 = getSxy( 4, 0 );
    double s30 = getSxy( 3, 0 );
    double s20 = getSxy( 2, 0 );
    double s10 = getSxy( 1, 0 );
    double s00 = (double)m_dataPoints.size();

    double s21 = getSxy( 2, 1 );
    double s11 = getSxy( 1, 1 );
    double s01 = getSxy( 0, 1 );

    // Da / D
    m_a = (s21 * (s20 * s00 - s10 * s10) - s11 * (s30 * s00 - s10 * s20) + s01 * (s30 * s10 - s20 * s20))
                /
                (s40 * (s20 * s00 - s10 * s10) - s30 * (s30 * s00 - s10 * s20) + s20 * (s30 * s10 - s20 * s20));

    return m_a;
}

///-------------------------------------------------------------------------------------------------
/// \fn	double CCurveFit::GetBTerm();
///
/// \brief	Gets the b term of the equation ax^2 + bx + c.
///
/// \return	The b term.
///-------------------------------------------------------------------------------------------------
double QuadCurveFitUtility::getBTerm()
{
    if( m_dataPoints.size() < MIN_VALUES )
    {
        std::cout<<"Atleast 3 data sets required to interpolate" ;
        return 0;
    }


    // notation sjk to mean the sum of x_i^j_i^k.
    double s40 = getSxy( 4, 0 );
    double s30 = getSxy( 3, 0 );
    double s20 = getSxy( 2, 0 );
    double s10 = getSxy( 1, 0 );
    double s00 = (double)m_dataPoints.size();

    double s21 = getSxy( 2, 1 );
    double s11 = getSxy( 1, 1 );
    double s01 = getSxy( 0, 1 );

    //   Db / D
    m_b = (s40 * (s11 * s00 - s01 * s10) - s30 * (s21 * s00 - s01 * s20) + s20 * (s21 * s10 - s11 * s20))
        /
        (s40 * (s20 * s00 - s10 * s10) - s30 * (s30 * s00 - s10 * s20) + s20 * (s30 * s10 - s20 * s20));

    return m_b;
}

///-------------------------------------------------------------------------------------------------
/// \fn	double CCurveFit::GetCTerm();
///
/// \brief	Gets c term of the equation ax^2 + bx + c.
///
/// \return	The c term.
///-------------------------------------------------------------------------------------------------
double QuadCurveFitUtility::getCTerm()
{
    if( m_dataPoints.size() < MIN_VALUES )
    {
        std::cout<<"Atleast 3 data sets required to interpolate" ;
        return 0;
    }


    // notation sjk to mean the sum of x_i^j_i^k.
    double s40 = getSxy( 4, 0 );
    double s30 = getSxy( 3, 0 );
    double s20 = getSxy( 2, 0 );
    double s10 = getSxy( 1, 0 );
    double s00 = (double)m_dataPoints.size();

    double s21 = getSxy( 2, 1 );
    double s11 = getSxy( 1, 1 );
    double s01 = getSxy( 0, 1 );

    //   Dc / D
    m_c = (s40*(s20 * s01 - s10 * s11) - s30*(s30 * s01 - s10 * s21) + s20*(s30 * s11 - s20 * s21))
            /
            (s40 * (s20 * s00 - s10 * s10) - s30 * (s30 * s00 - s10 * s20) + s20 * (s30 * s10 - s20 * s20));

    return m_c;
}

///-------------------------------------------------------------------------------------------------
/// \fn	double CCurveFit::GetRSquare();
///
/// \brief	Gets r squared.
///
/// \return	The r squared value.
///-------------------------------------------------------------------------------------------------
double QuadCurveFitUtility::getRSquare()
{
    if( m_dataPoints.size() < MIN_VALUES )
    {
        std::cout<<"Atleast 3 data sets required to interpolate" ;
        return 0;
    }

    return (1.0 - getSSerr() / getSStot());

}

// helper functions

///-------------------------------------------------------------------------------------------------
/// \fn	double CCurveFit::getSxy( int nXPower, int nYPower );
///
/// \brief	Gets sum if x^nXPower * y^nYPower.
///
/// \param	nXPower	The x power.
/// \param	nYPower	The y power.
///
/// \return	The sxy.
///-------------------------------------------------------------------------------------------------
double QuadCurveFitUtility::getSxy( int _nX, int _nY )
{
    double dSxy = 0.0;

    for(int i = 0;i<m_dataPoints.size();i++)
    {
        dSxy += pow(m_dataPoints[i].x,_nX) * pow(m_dataPoints[i].y,_nY);
    }

    return dSxy;
}

///-------------------------------------------------------------------------------------------------
/// \fn	double CCurveFit::getYMean()
///
/// \brief	Get y coordinate mean.
///
/// \return	The calculated y coordinate mean.
///-------------------------------------------------------------------------------------------------
double QuadCurveFitUtility::getYMean()
{
    double dTotal = 0.0;
    int nCount = 0;
    for(int i = 0;i<m_dataPoints.size();i++)
    {
        dTotal += m_dataPoints[i].y;
        nCount++;
    }

    return dTotal / nCount;
}

///-------------------------------------------------------------------------------------------------
/// \fn	double CCurveFit::getSStot()
///
/// \brief	Gets s stot.
///
/// \return	The s stot.
///-------------------------------------------------------------------------------------------------
double QuadCurveFitUtility::getSStot()
{
    double ssTot = 0.0;
    double dYMean = getYMean();

    for(int i = 0;i<m_dataPoints.size();i++)
    {
        ssTot += pow( (m_dataPoints[i].y * dYMean), 2 );
    }

    return ssTot;
}

///-------------------------------------------------------------------------------------------------
/// \fn	double CCurveFit::getSSerr()
///
/// \brief	Gets s serr.
///
/// \return	The s serr.
///-------------------------------------------------------------------------------------------------
double QuadCurveFitUtility::getSSerr()
{
    double ssErr = 0.0;

    //auto pThis = this;

    for(int i = 0;i<m_dataPoints.size();i++)
    {
        ssErr += pow( (m_dataPoints[i].y - getPredictedY( m_dataPoints[i].x)), 2 );
    }

    return ssErr;
}

///-------------------------------------------------------------------------------------------------
/// \fn	double CCurveFit::getPredictedY( double x )
///
/// \brief	Gets predicted y coordinate.
///
/// \param	x	The double to process.
///
/// \return	The predicted y coordinate.
///-------------------------------------------------------------------------------------------------
double QuadCurveFitUtility::getPredictedY( double _x )
{
    return (getATerm() * pow( _x, 2 )) + (getBTerm() * _x) + getCTerm();
}

double QuadCurveFitUtility::getDifferential( double _x )
{
    //2at + b is the differential
    return ((2 * getATerm() *  _x) + (getBTerm()));
}

void QuadCurveFitUtility::processPoints()
{
    int i = m_dataPoints.size();

    //std::sort(m_dataPoints)
    std::cout<<"size is:"<<i<<"first:"<<m_dataPoints[0].x<<"::"<<m_dataPoints[0].y<<"last:"<<m_dataPoints[i - 1].x<<"::"<<m_dataPoints[i - 1].y<<"\n";
    double temp = 0;
    if(i != 0)
    {
        temp = m_dataPoints[0].x;
        while(temp < m_dataPoints[i - 1].x)
        {
            std::cout<<" Interpol. X and Y are: "<<temp<<":"<<getPredictedY(temp);
            m_dataPointsAfterInterpol.push_back(cv::Point2f(temp,getPredictedY(temp)));
            temp = temp + 0.01;
        }
    }
}


void QuadCurveFitUtility::processParametricPoints(std::vector<float>& _inputBuffer)
{
    int i = m_dataPoints.size();

    //std::sort(m_dataPoints)
    std::cout<<"size is:"<<i<<"first:"<<m_dataPoints[0].x<<"::"<<m_dataPoints[0].y<<"last:"<<m_dataPoints[i - 1].x<<"::"<<m_dataPoints[i - 1].y<<"\n";
    double temp = 0;
    if(i != 0)
    {
        temp = m_dataPoints[0].x;
        while(temp <= 1.0)
        {
            std::cout<<" Interpol. X and Y are: "<<temp<<":"<<getPredictedY(temp);
            //m_dataPointsAfterInterpol.push_back(cv::Point2f(temp,getPredictedY(temp)));

            _inputBuffer.push_back(getPredictedY(temp));
            temp = temp + 0.01;
        }
    }
}

void QuadCurveFitUtility::processParametricPoints3D(std::vector<float>& _inputBuffer,float& o_coeffX)
{
    int i = m_dataPoints.size();

    //std::sort(m_dataPoints)
    std::cout<<"size is:"<<i<<"first:"<<m_dataPoints[0].x<<"::"<<m_dataPoints[0].y<<"last:"<<m_dataPoints[i - 1].x<<"::"<<m_dataPoints[i - 1].y<<"\n";
    double temp = 0;
    if(i != 0)
    {
        temp = m_dataPoints[0].x;
        while(temp <= 1.0)
        {
            std::cout<<" Interpol. X and Y are: "<<temp<<":"<<getPredictedY(temp);
            //m_dataPointsAfterInterpol.push_back(cv::Point2f(temp,getPredictedY(temp)));

            float tempValue = getPredictedY(temp);
            if(!(std::isnan(tempValue)))
            {
                _inputBuffer.push_back(getPredictedY(temp));
            }
            temp = temp + 0.01;
        }

        // get the differential at t = 1.0 which will be our
        // tangent to the impact point
        o_coeffX = getDifferential(1.0);

    }
}

