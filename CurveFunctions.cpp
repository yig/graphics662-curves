#include "CurveFunctions.h"
using std::vector;

#include <cassert>
#include <cmath>

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
    
    MatrixXd ts(1,4);
    ts(0,0) = t*t*t;
    ts(1,0) = t*t;
    ts(2,0) = t;
    ts(3,0) = 1;
    
    MatrixXd ps(4,2);
    ps.row(0) = p0;
    ps.row(1) = p1;
    ps.row(2) = p2;
    ps.row(3) = p3;
    
    return ts * m * ps;
    
    /*
    Point p = (-1*p0 + 3*p1 - 3*p2 + 1*p3)*t*t*t 
		    + ( 3*p0 - 6*p1 + 3*p2)*t*t
		    + (-3*p0 + 3*p1)*t
		    + 1.0*p0;
	*/
	
	return p;
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
    std::vector< Point > result;
    // Reserve some space.
    result.reserve( samplesPerCurve*(controlPoints.size()-1)/3 + 1 );
    // Evaluate each curve.
    for( int i = 0; i+3 < controlPoints.size(); ++i )
    {
        const Point* p0 = &controlPoints.at(i);
        for( int ti = 0; ti < samplesPerCurve; ++ti )
        {
            const real_t t = float(ti)/samplesPerCurve;
            result.push_back( EvaluateCubicBezierCurve( p0, p0+1, p0+2, p0+3, t, approach ) );
        }
    }
    result.push_back( controlPoints.back() );
    return result;
}

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
    std::vector< Point > result;
    // Reserve some space.
    result.reserve( samplesPerCurve*(controlPoints.size()-1)/3 + 1 );
    // Evaluate each curve.
    for( int i = 0; i+3 < controlPoints.size(); ++i )
    {
        const Point* p0 = &controlPoints.at(i);
        for( int ti = 0; ti < samplesPerCurve; ++ti )
        {
            const real_t t = float(ti)/samplesPerCurve;
            result.push_back( EvaluateCubicHermiteCurve( p0, p0+1, p0+2, p0+3, t ) );
        }
    }
    result.push_back( controlPoints.back() );
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
        controlPoints[2*dim + 1] = Point(C(i,0), C(i,1));
    }
}


// Evaluate a cubic B-Spline with control points 'controlPoints' arranged:
//     p0 p1 p2 ( p3 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
std::vector< Point > EvaluateCubicBSpline( const std::vector< Point >& controlPoints, const int samplesPerCurve );
// Evaluate a cubic B-Spline curve at location 't'.
Point EvaluateCubicBSplineCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t );

// Compute cubic BSpline control points that interpolate the given points.
std::vector< Point > ComputeBSplineFromInterpolatingPoints( const std::vector< Point >& interpPoints )
{
    assert( interpPoints.size() >= 2 );
    
    // ADD YOUR CODE HERE
    std::vector< Point > result;
    int totalPoints = interpPoints.size();
    
	// Prepare data
	if (totalPoints < 2)
		return result;
	int degree = 3;
	int dim = totalPoints + 2;
	MatrixXd A(dim, dim);
	MatrixXd C(dim, 2);
	MatrixXd P(dim, 2);
    
    // Compute coefficients
    std::vector<float> L;
    for (int i = 0; i < totalPoints + degree * 2; i++){
        L.push_back(float(i - degree));	
    }
    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            A(i,j) = 0.0;
        }
    }

    // These computeDN and computeN are recursive functions
    A(0,0) = computeDN(3,0,0,2); A(0,1) = computeDN(3,1,0,2); A(0,2) = computeDN(3,2,0,2); A(0,3) = computeDN(3,3,0,2);
    for (int i = 1; i < dim - 2; i++){
        A(i,i-1) = computeN(3, i - 1, i - 1); 
        A(i, i) = computeN(3, i, i - 1);
        A(i, i+1) = computeN(3, i + 1, i - 1);
        A(i, i+2) = computeN(3, i + 2, i - 1);
    }
    A(dim - 2, dim - 4) = computeN(3, dim - 4, dim - 3);
    A(dim - 2, dim - 3) = computeN(3, dim - 3, dim - 3);
    A(dim - 2, dim - 2) = computeN(3, dim - 2, dim - 3);
    A(dim - 2, dim - 1) = computeN(3, dim - 1, dim - 3);
    A(dim - 1, dim - 4) = computeDN(3, dim - 4, dim - 3, 2); 
    A(dim - 1, dim - 3) = computeDN(3, dim - 3, dim - 3, 2);
    A(dim - 1, dim - 2) = computeDN(3, dim - 2, dim - 3, 2); 
    A(dim - 1, dim - 1) = computeDN(3, dim - 1, dim - 3, 2);
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
    
    std::vector< Point > result;
    result.push_back( EvaluateCubicBSplineCurve( controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3], .5 ) );
    // The middle curves should be evaluated at 0.
    // NOTE: We iterate until i+3 < controlPoints.size()-1, which is one before the last curve.
    //       size() is an unsigned quantity, so we don't want to ever subtract from it,
    //       because negative numbers underflow.
    for( int i = 1; i+4 < controlPoints.size(); ++i )
    {
        result.push_back( EvaluateCubicBSplineCurve( controlPoints[i], controlPoints[i+1], controlPoints[i+2], controlPoints[i+3], 0. ) );
    }
    // The last curve should be evaluated at .5.
    result.push_back( EvaluateCubicBSplineCurve( controlPoints[i], controlPoints[i+1], controlPoints[i+2], controlPoints[i+3], .5 ) );
}

}
