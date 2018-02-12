#include "CurveFunctions.h"
using std::vector;

#include <Eigen/LU>
using Eigen::MatrixXd;
using Eigen::Matrix4d;
using Eigen::Vector4d;

#include <cassert>
#include <cmath>

#include "jsassert.h"
#undef assert
#define assert(cond) jsAssert(cond)

// Call these to raise a dialog box or log to the javascript console for debugging.
// NOTE: You can pass either a const char* or an std::string.
// void jsAlert( const std::string& msg );
// void jsLog( const std::string& msg );
// void jsWarn( const std::string& msg );
// void jsError( const std::string& msg );

namespace
{
// For debugging, this function will return a string given a Point.
std::string point2string( const Point& p ) {
    return "( " + std::to_string(p(0)) + ", " + std::to_string(p(1)) + " )";
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
    if( BernsteinApproach == approach ) return EvaluateCubicBezierCurveBernstein( p0, p1, p2, p3, t );
    else if( MatrixApproach == approach ) return EvaluateCubicBezierCurveMatrix( p0, p1, p2, p3, t );
    else if( CasteljauApproach == approach ) return EvaluateCubicBezierCurveCasteljau( p0, p1, p2, p3, t );
    else {
        assert( !"Unknown EvaluateCubicBezierCurveApproach" );
        return Point(-31337,-31337);
    }
}
Point EvaluateCubicBezierCurveBernstein( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t )
{
    // ADD YOUR CODE HERE
    Point result
        = (1-t) * (1-t) * (1-t) * p0
        + 3 * t * (1-t) * (1-t) * p1
        + 3 * t * t * (1-t) * p2
        + t * t * t * p3;
    return result;
}
Point EvaluateCubicBezierCurveMatrix( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t )
{
    // ADD YOUR CODE HERE
    Matrix4d m;
    m.setZero();
    m(0,0) = -1;
    m(0,1) = 3;
    m(0,2) = -3;
    m(0,3) = 1;
    m(1,0) = 3;
    m(1,1) = -6;
    m(1,2) = 3;
    m(2,0) = -3;
    m(2,1) = 3;
    m(3,0) = 1;
    
    Vector4d ts;
    ts(0) = t*t*t;
    ts(1) = t*t;
    ts(2) = t;
    ts(3) = 1;
    
    MatrixXd ps(4,2);
    ps.row(0) = p0;
    ps.row(1) = p1;
    ps.row(2) = p2;
    ps.row(3) = p3;
    
    return ps.transpose() * m * ts;
    
    /*
    Point p = (-1*p0 + 3*p1 - 3*p2 + 1*p3)*t*t*t 
            + ( 3*p0 - 6*p1 + 3*p2)*t*t
            + (-3*p0 + 3*p1)*t
            + 1.0*p0;
    
    return p;
    */
}
Point EvaluateCubicBezierCurveCasteljau( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t )
{
    // ADD YOUR CODE HERE
    Point tmp0, tmp1, tmp2;
    tmp0 = p0 * (1-t) + p1 * t;
    tmp1 = p1 * (1-t) + p2 * t;
    tmp2 = p2 * (1-t) + p3 * t;
    tmp0 = tmp0 * (1-t) + tmp1 * t;
    tmp1 = tmp1 * (1-t) + tmp2 * t;
    tmp0 = tmp0 * (1-t) + tmp1 * t;
    return tmp0;
}

// Evaluate a cubic Bezier spline with control points 'controlPoints' arranged
//     on_curve ( off_curve off_curve on_curve )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Returns the sampled points.
std::vector< Point > EvaluateCubicBezierSpline( const std::vector< Point >& controlPoints, const int samplesPerCurve, EvaluateCubicBezierCurveApproach approach )
{
    assert( controlPoints.size() >= 4 );
    assert( samplesPerCurve > 0 );
    
    // ADD YOUR CODE HERE
    const std::vector< Point >& C = controlPoints;
    std::vector< Point > result;
    // Reserve some space.
    result.reserve( samplesPerCurve*(C.size()-1)/3 + 1 );
    // Evaluate each curve.
    for( int i = 0; i+3 < C.size(); i += 3 )
    {
        for( int ti = 0; ti < samplesPerCurve; ++ti )
        {
            const real_t t = real_t(ti)/samplesPerCurve;
            result.push_back( EvaluateCubicBezierCurve( C[i], C[i+1], C[i+2], C[i+3], t, approach ) );
        }
    }
    // Bezier splines interpolate, so the last point is the last control point.
    result.push_back( C.back() );
    return result;
}

/// ======================================================================================

// Evaluate a cubic Hermite curve at location 't'.
Point EvaluateCubicHermiteCurve( const Point& p0, const Point& dp0, const Point& p1, const Point& dp1, const real_t t )
{
    // ADD YOUR CODE HERE
    Point p =
         p0 * ( 2*t*t*t - 3*t*t + 1 ) +
        dp0 * ( t*t*t - 2*t*t + t ) +
         p1 * ( -2*t*t*t + 3*t*t ) +
        dp1 * ( t*t*t - t*t )
        ;
    return p;
}

// Evaluate a cubic Hermite spline with control points 'controlPoints' arranged:
//     p0 derivative_at_p0 ( p1 derivative_at_p1 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
std::vector< Point > EvaluateCubicHermiteSpline( const std::vector< Point >& controlPoints, const int samplesPerCurve )
{
    assert( controlPoints.size() >= 4 );
    assert( samplesPerCurve > 0 );
    
    // ADD YOUR CODE HERE
    const std::vector< Point >& C = controlPoints;
    std::vector< Point > result;
    // Reserve some space.
    result.reserve( samplesPerCurve*( C.size()/2 - 1 ) + 1 );
    // Evaluate each curve.
    for( int i = 0; i+3 < C.size(); i += 2 )
    {
        for( int ti = 0; ti < samplesPerCurve; ++ti )
        {
            const real_t t = real_t(ti)/samplesPerCurve;
            result.push_back( EvaluateCubicHermiteCurve( C[i], C[i+1], C[i+2], C[i+3], t ) );
        }
    }
    // Hermite splines interpolate, so the last point is the last interpolated control point.
    assert( C.size() >= 2 );
    result.push_back( C[C.size()-2] );
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
void CalculateHermiteSplineDerivativesForC2Continuity( std::vector< Point >& controlPoints )
{
    // Do nothing if there aren't enough control points.
    if( controlPoints.size() < 4 ) return;
    
    assert( controlPoints.size() >= 4 );
    assert( controlPoints.size() % 2 == 0 );
    
    // ADD YOUR CODE HERE
    int dim = controlPoints.size()/2;
    MatrixXd A(dim, dim);
    MatrixXd C(dim, 2);
    MatrixXd P(dim, 2);
    
    
    // Initialize the matrix to zeros.
    A.setZero(dim, dim);
    
    
    // Regular row equations (from 1 to dim-1).
    for( int i = 1; i < dim-1; ++i )
    {
        A( i,i-1 ) = 1;
        A( i,i   ) = 4;
        A( i,i+1 ) = 1;
        
        P.row( i ) = 3*( controlPoints.at( 2*(i+1) ) - controlPoints.at( 2*(i-1) ) );
    }
    
    
    // The boundary equations are the last two equations.
    const bool kNaturalBoundaries = true;
    // Determine the boundary slope naturally (second derivative = 0).
    if( kNaturalBoundaries )
    {
        // Starting boundary
        A( 0, 0 ) = 2;
        A( 0, 1 ) = 1;
        P.row( 0 ) = 3*( controlPoints.at( 2 ) - controlPoints.at( 0 ) );
        
        // Ending boundary
        A( dim-1, dim-2 ) = 1;
        A( dim-1, dim-1 ) = 2;
        P.row( dim-1 ) = 3*( controlPoints.at( 2*(dim-1) ) - controlPoints.at( 2*(dim-2) ) );
    }
    // Determine the boundary slope by preserving the current slope.
    else
    {
        // First derivatives are specified by endPoints.
        A( 0, 0 ) = 1;
        P.row( 0 ) = controlPoints.at(1);
        
        A( dim-1, dim-1 ) = 1;
        P.row( dim-1 ) = controlPoints.back();
    }
    
    
    // Solve the system of equations.
    C = A.fullPivLu().solve(P);
    for( int i = 0; i < dim; ++i )
    {
        controlPoints.at( 2*i + 1 ) = Point( C(i,0), C(i,1) );
    }
}

/// ======================================================================================

// Evaluate a Catmull-Rom Spline with control points 'controlPoints' arranged:
//     p0 p1 p2 ( p3 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
std::vector< Point > EvaluateCatmullRomSpline( const std::vector< Point >& controlPoints, const int samplesPerCurve, const real_t alpha )
{
    assert( controlPoints.size() >= 4 );
    assert( samplesPerCurve > 0 );
    
    // ADD YOUR CODE HERE
    std::vector< Point > C;
    // p0 p0 p1 p2 ... pN pN
    C.push_back( controlPoints.front() );
    C.insert( C.end(), controlPoints.begin(), controlPoints.end() );
    C.push_back( controlPoints.back() );
    // Now reflect the first and last points.
    C[0] = C[1] + (C[1] - C[2]);
    C[C.size()-1] = C[C.size()-2] + (C[C.size()-2] - C[C.size()-3]);
    
    std::vector< Point > result;
    // Reserve some space.
    result.reserve( samplesPerCurve*(C.size()-3) + 1 );
    // Evaluate each curve.
    for( int i = 0; i+3 < C.size(); ++i )
    {
        for( int ti = 0; ti < samplesPerCurve; ++ti )
        {
            const real_t t = real_t(ti)/samplesPerCurve;
            result.push_back( EvaluateCatmullRomCurve( C[i], C[i+1], C[i+2], C[i+3], t, alpha ) );
        }
    }
    // Catmull-Rom splines interpolate, so the last point is the last control point.
    result.push_back( controlPoints.back() );
    return result;
}
// Evaluate a cubic Catmull-Rom Spline curve at location 't'.
Point EvaluateCatmullRomCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t, const real_t alpha )
{
    // ADD YOUR CODE HERE
    
    // Implemented with a matrix:
    Matrix4d M;
    M << -1, 3, -3, 1,
        2, -5, 4, -1,
        -1, 0, 1, 0,
        0, 2, 0, 0;
    
    Vector4d power( t*t*t, t*t, t, 1 );
    
    Vector4d w = ( power.transpose() * M ).transpose();
    
    return alpha*( w(0)*p0 + w(1)*p1 + w(2)*p2 + w(3)*p3 );
    
    // Implemented via Cubic Bezier conversion:
    const real_t d1 = (p1-p0).norm();
    const real_t d2 = (p2-p1).norm();
    const real_t d3 = (p3-p2).norm();
    
    const real_t d1a = pow( d1, alpha );
    const real_t d2a = pow( d2, alpha );
    const real_t d3a = pow( d3, alpha );
    
    // Convert to a Bezier curve according to
    // "On the Parameterization of Catmull-Rom Curves" by Yuksel et al. 2009.
    const Point b0 = p1;
    const Point b1 = ( d1a*d1a*p2 - d2a*d2a*p0 + ( 2*d1a*d1a + 3*d1a*d2a + d2a*d2a )*p1 )/( 3*d1a*( d1a + d2a ) );
    const Point b2 = ( d3a*d3a*p1 - d2a*d2a*p3 + ( 2*d3a*d3a + 3*d3a*d2a + d2a*d2a )*p2 )/( 3*d3a*( d3a + d2a ) );
    const Point b3 = p2;
    
    return EvaluateCubicBezierCurveBernstein( b0, b1, b2, b3, t );
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
std::vector< Point > EvaluateCubicBSpline( const std::vector< Point >& controlPoints, const int samplesPerCurve )
{
    assert( controlPoints.size() >= 4 );
    assert( samplesPerCurve > 0 );
    
    // ADD YOUR CODE HERE
    const std::vector< Point >& C = controlPoints;
    std::vector< Point > result;
    // Reserve some space.
    result.reserve( samplesPerCurve*(C.size()-3) + 1 );
    // Evaluate each curve.
    for( int i = 0; i+3 < C.size(); ++i )
    {
        for( int ti = 0; ti < samplesPerCurve; ++ti )
        {
            const real_t t = real_t(ti)/samplesPerCurve;
            result.push_back( EvaluateCubicBSplineCurve( C[i], C[i+1], C[i+2], C[i+3], t ) );
        }
    }
    
    // The last point.
    int i = controlPoints.size()-4;
    assert( i+3 < C.size() );
    result.push_back( EvaluateCubicBSplineCurve( C[i], C[i+1], C[i+2], C[i+3], 1. ) );
    
    return result;
}
// Evaluate a cubic B-Spline curve at location 't'.
Point EvaluateCubicBSplineCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t )
{
    // ADD YOUR CODE HERE
    return (1./6.)*(
        p0*( 1 - 3*t + 3*t*t - t*t*t ) +
        p1*( 4 - 6*t*t + 3*t*t*t ) +
        p2*( 1 + 3*t + 3*t*t - 3*t*t*t ) +
        p3*( t*t*t )
        );
}

// Compute cubic BSpline control points that interpolate the given points.
std::vector< Point > ComputeBSplineFromInterpolatingPoints( const std::vector< Point >& interpPoints )
{
    assert( interpPoints.size() >= 2 );
    
    // ADD YOUR CODE HERE
    std::vector< Point > result;
    
    // Prepare data
    const int N = interpPoints.size();
    if( N < 2 ) return result;
    
    MatrixXd A, RHS;
    A.setZero( N+2, N+2 );
    RHS.setZero( N+2, 2 );
    
    // The right-hand-side is the interpolated points followed by zeros.
    for( int i = 0; i < N; ++i )
    {
        A( i,i ) = 1./6.;
        A( i,i+1 ) = 4./6.;
        A( i,i+2 ) = 1./6.;
        
        RHS.row(i) = interpPoints.at(i);
    }
    
    // The last two rows are the natural (second derivative = 0) constraints.
    A( N, 0 ) = 1;
    A( N, 1 ) = -2;
    A( N, 2 ) = 1;
    
    A( N+1, N+2-1 ) = 1;
    A( N+1, N+2-2 ) = -2;
    A( N+1, N+2-3 ) = 1;
    
    const MatrixXd C = A.fullPivLu().solve(RHS);
    for( int i = 0; i < N+2; ++i )
    {
        result.push_back( C.row(i) );
    }
    return result;
}
// Given a sequence of cubic BSpline control points, returns the interpolating points
// that could have been used to create them via ComputeBSplineFromInterpolatingPoints().
std::vector< Point > ComputeInterpolatingPointsFromBSpline( const std::vector< Point >& controlPoints )
{
    assert( controlPoints.size() >= 4 );
    const std::vector< Point >& C = controlPoints;
    
    std::vector< Point > result;
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
