#ifndef __CurveFunctions_h__
#define __CurveFunctions_h__

#include <vector>
#include <Eigen/Core>

namespace Curve
{

typedef Eigen::Vector2d Point;
typedef double real_t;

// Evaluate a cubic Bezier spline with control points 'controlPoints' arranged
//     on_curve ( off_curve off_curve on_curve )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
void EvaluateCubicBezierSplineBernstein( const std::vector< Point >& controlPoints, const int samplesPerCurve, std::vector< Point >& curvePointsOut );
void EvaluateCubicBezierSplineMatrix( const std::vector< Point >& controlPoints, const int samplesPerCurve, std::vector< Point >& curvePointsOut );
void EvaluateCubicBezierSplineCasteljau( const std::vector< Point >& controlPoints, const int samplesPerCurve, std::vector< Point >& curvePointsOut );

// Evaluate a cubic Bezier curve at location 't'.
Point EvaluateCubicBezierCurveBernstein( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t );
Point EvaluateCubicBezierCurveMatrix( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t );
Point EvaluateCubicBezierCurveCasteljau( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t );

// Evaluate a cubic Hermite spline with control points 'controlPoints' arranged:
//     p0 derivative_at_p0 ( p1 derivative_at_p1 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
void EvaluateCubicHermiteSpline( const std::vector< Point >& controlPoints, const int samplesPerCurve, std::vector< Point >& curvePointsOut );
// Evaluate a cubic Hermite curve at location 't'.
Point EvaluateCubicHermiteCurve( const Point& p0, const Point& dp0, const Point& p1, const Point& dp1, const real_t t );

// Evaluate a cubic B-Spline with control points 'controlPoints' arranged:
//     p0 p1 p2 ( p3 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
void EvaluateCubicBSpline( const std::vector< Point >& controlPoints, const int samplesPerCurve, std::vector< Point >& curvePointsOut );
// Evaluate a cubic B-Spline curve at location 't'.
Point EvaluateCubicBSplineCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t );

// Given a cubic Hermite spline with control points 'controlPoints' arranged:
//     p0 derivative_at_p0 ( p1 derivative_at_p1 )+
// replaces the derivative entries with values that result in a C2 continuous
// Hermite spline.
// NOTE: 'controlPoints' is an input and output parameter. The derivative entries are replaced.
void CalculateHermiteSplineDerivativesForC2Continuity( std::vector< Point >& controlPoints );

}

#endif /* __CurveFunctions_h__ */
