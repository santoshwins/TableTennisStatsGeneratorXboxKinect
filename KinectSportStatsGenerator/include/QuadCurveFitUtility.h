#ifndef QUADCURVEFIT_H
#define QUADCURVEFIT_H
#include<vector>
#include "opencv2/opencv.hpp"

/// @file QuadCurveUtility.h
/// @brief data class for players
/// @author Santosh
/// @version 1.0
/// @date 16/08/13
/// @class QuadCurveUtility
/// @brief contains cramers rule
class QuadCurveFitUtility
{
public:
    QuadCurveFitUtility();
    ~QuadCurveFitUtility();

    std::vector<cv::Point2f>& getInterPolPts();

    // attributes
protected:
    std::vector<cv::Point2f> m_dataPoints;
    std::vector<cv::Point2f> m_dataPointsAfterInterpol;

    std::vector<cv::Point3f> m_hermiteInterPolPts;
    std::vector<cv::Point3f> m_dataPointsHermite;

    double m_a;
    double m_b;
    double m_c;


    float m_steps;
    int m_noOfPoints;

public:
    void addPoints( cv::Point2f _inputSet );
    void processPoints();

    void processParametricPoints(std::vector<float>& _inputBuffer);

    void processParametricPoints3D(std::vector<float>& _inputBuffer,float& o_coeffX);


    // operations
public:
    // Gets the a term of the equation ax^2 + bx + c.
    double getATerm();

    // Gets the b term of the equation ax^2 + bx + c.
    double getBTerm();

    // Gets c term of the equation ax^2 + bx + c.
    double getCTerm();

    // Gets r squared value. goodness measure
    double getRSquare();

protected:
    // helper functions

    // Gets sum of x^nX * y^nY
    double getSxy( int _nX, int _nY );
    double getYMean();
    double getSStot();
    double getSSerr();
    double getPredictedY( double _x );

    void calculateHermiteCurvePts(cv::Point3f _startPt, cv::Point3f _endPt, cv::Point3f _startTangent, cv::Point3f _endTangent, float _inputInit, float _interval);



    std::vector<cv::Vec4d> splinterp(std::vector<float> x);

    double getDifferential( double _x );

};

#endif // QUADCURVEFIT_H
