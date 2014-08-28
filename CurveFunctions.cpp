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
extern void jsAlert( const std::string& msg );
extern void jsLog( const std::string& msg );

namespace Curve
{

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
    Point p = (2 * t * t * t - 3 * t * t + 1) * p0 
		    + (t * t * t - 2 * t * t + t) * dp0
			+ (-2 * t * t * t + 3 * t * t) * p1
			+ (t * t * t - t * t) * dp1;
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
    result.reserve( samplesPerCurve*(C.size()-1)/3 + 1 );
    // Evaluate each curve.
    for( int i = 0; i+3 < C.size(); i += 3 )
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
//	MatrixXd A(3,3);
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
    assert( controlPoints.size() >= 4 );
    assert( controlPoints.size() % 2 == 0 );
    
    // ADD YOUR CODE HERE
    int dim = controlPoints.size()/2;
	MatrixXd A(dim, dim);
	MatrixXd C(dim, 2);
	MatrixXd P(dim, 2);
    
    // Zero A:
    A.setZero(dim, dim);
    
    // Determine the boundary slope either by endPoints or automatically
    if( true )
    {
        // Additional constraints: second derivative are zero at end points
        // first row	
        A(0,0) = 2;
        A(0,1) = 1;
        P.row(0) = 3 * (controlPoints[2] - controlPoints[0]);
        
        // last row
        A(dim-1, dim-2) = 1;
        A(dim-1, dim-1) = 2;
        P.row(dim-1) =  3 * (controlPoints[2*(dim-1)] - controlPoints[2*(dim-2)]);
    }
    else
    {
        // First derivatives are specified by endPoints.
        A(0,0) = 1;
        P.row(0) = controlPoints[1];
        
        A(dim-1,dim-1) = 1;
        P.row(dim-1) = controlPoints.back();
    }
    
    //middle rows
    for (int i = 1; i < dim - 1; i++)
    {
        A(i, i-1) = 1; A(i, i) = 4; A(i, i+1) = 1;
    }
    for (int i = 1; i < dim - 1; i++)
    {
        P.row(i) = 3 * (controlPoints[2*(i+1)] - controlPoints[2*(i-1)]);
    }
    
    C = A.fullPivLu().solve(P);
    for (int i = 0; i < dim; i++){
        controlPoints.at(2*dim + 1) = Point(C(i,0), C(i,1));
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
    result.reserve( samplesPerCurve*(C.size()-1)/3 + 1 );
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
// ADD YOUR CODE HERE
real_t computeN( const std::vector< real_t >& L, int n, int j, real_t t )
{
	if (t < L[j] || t >= L[j+1+n])
		return 0.0;
	if (n == 0){
		if (t >= L[j] && t < L[j+1])
			return 1.0;
		else
			return 0.0;
	}
	else{
		real_t v1 = (t - L[j]) / (L[j + n] - L[j]) * computeN(L, n-1, j, t);
		real_t v2 = (L[j+n+1] - t) / (L[j + n + 1] - L[j +1]) * computeN(L, n-1, j+1, t);
		return v1 + v2;
	}
}


real_t computeDN( const std::vector< real_t >& L, int n, int j, int t, int d )
{
	if (d == 0){
		return computeN(L, n,j,t);
	}else{
		real_t v1 = 1 / (L[j+n] - L[j]) * computeDN(L, n-1, j, t, d-1);
		real_t v2 = 1 / (L[j+n+1] - L[j+1]) * computeDN(L, n-1, j+1, t, d-1);
		return n * (v1 - v2);
	}
}
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
    result.reserve( samplesPerCurve*(C.size()-1)/3 + 1 );
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
    int totalPoints = interpPoints.size();
    
	// Prepare data
	if (totalPoints < 2) return result;
	int degree = 3;
	int dim = totalPoints + 2;
	MatrixXd A(dim, dim);
	MatrixXd C(dim, 2);
	MatrixXd P(dim, 2);
    
    // Compute coefficients
    std::vector< real_t > L;
    for (int i = 0; i < totalPoints + degree * 2; i++){
        L.push_back(i - degree);
    }
    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            A(i,j) = 0.0;
        }
    }

    // These computeDN and computeN are recursive functions
    A(0,0) = computeDN(L,3,0,0,2); A(0,1) = computeDN(L,3,1,0,2); A(0,2) = computeDN(L,3,2,0,2); A(0,3) = computeDN(L,3,3,0,2);
    for (int i = 1; i < dim - 2; i++){
        A(i,i-1) = computeN(L, 3, i - 1, i - 1); 
        A(i, i) = computeN(L, 3, i, i - 1);
        A(i, i+1) = computeN(L, 3, i + 1, i - 1);
        A(i, i+2) = computeN(L, 3, i + 2, i - 1);
    }
    A(dim - 2, dim - 4) = computeN(L, 3, dim - 4, dim - 3);
    A(dim - 2, dim - 3) = computeN(L, 3, dim - 3, dim - 3);
    A(dim - 2, dim - 2) = computeN(L, 3, dim - 2, dim - 3);
    A(dim - 2, dim - 1) = computeN(L, 3, dim - 1, dim - 3);
    A(dim - 1, dim - 4) = computeDN(L, 3, dim - 4, dim - 3, 2); 
    A(dim - 1, dim - 3) = computeDN(L, 3, dim - 3, dim - 3, 2);
    A(dim - 1, dim - 2) = computeDN(L, 3, dim - 2, dim - 3, 2); 
    A(dim - 1, dim - 1) = computeDN(L, 3, dim - 1, dim - 3, 2);
    P(0,0) = 0; P(0,1) = 0;
    for (int i = 1; i < totalPoints + 1; i++){
        P(i,0) = interpPoints[i-1].x();
        P(i,1) = interpPoints[i-1].y();
    }
    P(dim-1,0) = 0; P(dim-1,1) = 0;
    C = A.fullPivLu().solve(P);
    for (int i = 0; i < totalPoints + 2; i++){
        result.push_back(Point(C(i,0), C(i,1)));
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
    result.push_back( EvaluateCubicBSplineCurve( C[0], C[1], C[2], C[3], .5 ) );
    // The middle curves should be evaluated at 0.
    // NOTE: We iterate until i+3 < controlPoints.size()-1, which is one before the last curve.
    //       size() is an unsigned quantity, so we don't want to ever subtract from it,
    //       because negative numbers underflow.
    // Declare i outside of the for loop so we can use it to evaluate the last curve.
    int i;
    for( i = 1; i+4 < C.size(); ++i )
    {
        result.push_back( EvaluateCubicBSplineCurve( C[i], C[i+1], C[i+2], C[i+3], 0. ) );
    }
    // The last curve should be evaluated at .5.
    assert( i+3 < C.size() );
    result.push_back( EvaluateCubicBSplineCurve( C[i], C[i+1], C[i+2], C[i+3], .5 ) );
    
    return result;
}

}
