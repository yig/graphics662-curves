#include "CurveFunctions.h"
using std::vector;

#include <Eigen/LU>
using Eigen::MatrixXd;
using Eigen::Matrix4d;
using Eigen::Vector4d;

#include <string>
#include <format>

#include <cassert>
#include <cmath>

namespace
{
// For debugging, this function will return a string given a Point.
std::string point2string( const Curve::Point& p ) {
    return std::format( "( {}, {} )", p(0), p(1) );
}
}

namespace Curve
{

// Bezier helper functions.
namespace
{
// You may add helper functions here.
}

// Evaluate a cubic Bezier curve at location 't'.
Point EvaluateCubicBezierCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t, EvaluateCubicBezierCurveApproach approach )
{
    if( EvaluateCubicBezierCurveApproach::Bernstein == approach ) return EvaluateCubicBezierCurveBernstein( p0, p1, p2, p3, t );
    else if( EvaluateCubicBezierCurveApproach::Matrix == approach ) return EvaluateCubicBezierCurveMatrix( p0, p1, p2, p3, t );
    else if( EvaluateCubicBezierCurveApproach::Casteljau == approach ) return EvaluateCubicBezierCurveCasteljau( p0, p1, p2, p3, t );
    else {
        assert( !"Unknown EvaluateCubicBezierCurveApproach" );
        return Point(-31337,-31337);
    }
}
Point EvaluateCubicBezierCurveBernstein( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t )
{
    // ADD YOUR CODE HERE
    return Point(0,0);
}
Point EvaluateCubicBezierCurveMatrix( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t )
{
    // ADD YOUR CODE HERE
    return Point(0,0);
}
Point EvaluateCubicBezierCurveCasteljau( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t )
{
    // ADD YOUR CODE HERE
    return Point(0,0);
}

// Evaluate a cubic Bezier spline with control points 'controlPoints' arranged
//     on_curve ( off_curve off_curve on_curve )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Returns the sampled points.
Points EvaluateCubicBezierSpline( const Points& controlPoints, const int samplesPerCurve, EvaluateCubicBezierCurveApproach approach )
{
    assert( controlPoints.size() >= 4 );
    assert( samplesPerCurve > 0 );
    
    // ADD YOUR CODE HERE
    Points result;
    return result;
}

/// ======================================================================================

// Evaluate a cubic Hermite curve at location 't'.
Point EvaluateCubicHermiteCurve( const Point& p0, const Point& dp0, const Point& p1, const Point& dp1, const real_t t )
{
    // ADD YOUR CODE HERE
    return Point(0,0);
}

// Evaluate a cubic Hermite spline with control points 'controlPoints' arranged:
//     p0 derivative_at_p0 ( p1 derivative_at_p1 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
Points EvaluateCubicHermiteSpline( const Points& controlPoints, const int samplesPerCurve )
{
    assert( controlPoints.size() >= 4 );
    assert( samplesPerCurve > 0 );
    
    // ADD YOUR CODE HERE
    Points result;
    return result;
}

// Given a cubic Hermite spline with control points 'controlPoints' arranged:
//     p0 derivative_at_p0 ( p1 derivative_at_p1 )+
// replaces the derivative entries with values that result in a C2 continuous
// Hermite spline.
// NOTE: 'controlPoints' is an input and output parameter. The derivative entries are replaced.

// Hint: To implement C2 continuous Hermite splines, you need to write a system of equations like we did in class and solve them.
//       This is best done with a matrix.
//       Then you solve a linear system Ac = p, where p are the known interpolated points and c are the unknown derivatives.
//       I have included the Eigen matrix and linear algebra package that can solve linear systems.
//       Below is an example showing how to use the linear system solver.
//
//  MatrixXd A(3,3);
//  MatrixXd c(3,1);
//  MatrixXd p(3,1);
//  A(0,0) = 1.0; A(0,1) = 0.0; A(0,2) = 0.0;
//  A(1,0) = 0.0; A(1,1) = 1.0; A(1,2) = 0.0;
//  A(2,0) = 0.0; A(2,1) = 0.0; A(2,2) = 1.0;
//  p(0,0) = 1.0; p(1,0) = 2.0; p(2,0) = 3.0;
//  c = A.fullPivLu().solve(p);
//
//  The result will be stored in c as follows: c(0,0) = 1.0; c(1,0) = 2.0; c(3,0) = 3.0, which satisfies Ac = p.
void CalculateHermiteSplineDerivativesForC2Continuity( Points& controlPoints )
{
    // Do nothing if there aren't enough control points.
    if( controlPoints.size() < 4 ) return;
    
    assert( controlPoints.size() >= 4 );
    assert( controlPoints.size() % 2 == 0 );
    
    // ADD YOUR CODE HERE
}

/// ======================================================================================

// Evaluate a Catmull-Rom Spline with control points 'controlPoints' arranged:
//     p0 p1 p2 ( p3 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
Points EvaluateCatmullRomSpline( const Points& controlPoints, const int samplesPerCurve, const real_t alpha )
{
    assert( controlPoints.size() >= 4 );
    assert( samplesPerCurve > 0 );
    
    // ADD YOUR CODE HERE
    Points result;
    return result;
}
// Evaluate a cubic Catmull-Rom Spline curve at location 't'.
Point EvaluateCatmullRomCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t, const real_t alpha )
{
    // ADD YOUR CODE HERE
    return Point(0,0);
}

/// ======================================================================================

// B-Spline helper functions
namespace
{
}

// Evaluate a cubic B-Spline with control points 'controlPoints' arranged:
//     p0 p1 p2 ( p3 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
Points EvaluateCubicBSpline( const Points& controlPoints, const int samplesPerCurve )
{
    assert( controlPoints.size() >= 4 );
    assert( samplesPerCurve > 0 );
    
    // ADD YOUR CODE HERE
    Points result;
    return result;
}
// Evaluate a cubic B-Spline curve at location 't'.
Point EvaluateCubicBSplineCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t )
{
    // ADD YOUR CODE HERE
    return Point(0,0);
}

// Compute cubic BSpline control points that interpolate the given points.
Points ComputeBSplineFromInterpolatingPoints( const Points& interpPoints )
{
    assert( interpPoints.size() >= 2 );
    
    // ADD YOUR CODE HERE
    Points result;
    return result;
}
// Given a sequence of cubic BSpline control points, returns the interpolating points
// that could have been used to create them via ComputeBSplineFromInterpolatingPoints().
Points ComputeInterpolatingPointsFromBSpline( const Points& controlPoints )
{
    assert( controlPoints.size() >= 4 );
    const Points& C = controlPoints;
    
    Points result;
    // The interpolated points are at the start and end of each cubic B-Spline
    // (they are continuous).
    // So let's just sample the t=1 point on every cubic BSpline,
    // as well as the t=0 point of the first one.
    result.push_back( EvaluateCubicBSplineCurve( C[0], C[1], C[2], C[3], 0. ) );
    for( int i = 0; i+3 < C.size(); ++i )
    {
        result.push_back( EvaluateCubicBSplineCurve( C[i], C[i+1], C[i+2], C[i+3], 1. ) );
    }
    
    return result;
}

}
