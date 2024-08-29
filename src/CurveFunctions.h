#pragma once

#include <vector>
#include <Eigen/Core>

namespace Curve
{

typedef Eigen::Vector2d Point;
typedef std::vector< Point > Points;
typedef double real_t;


// Evaluate a cubic Bezier spline with control points 'controlPoints' arranged
//     on_curve ( off_curve off_curve on_curve )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Returns the sampled points.
enum class EvaluateCubicBezierCurveApproach
{
    Bernstein,
    Matrix,
    Casteljau
};
std::vector< Point > EvaluateCubicBezierSpline( const Points& controlPoints, const int samplesPerCurve, EvaluateCubicBezierCurveApproach approach );

// Evaluate a cubic Bezier curve at location 't'.
Point EvaluateCubicBezierCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t, EvaluateCubicBezierCurveApproach approach );
Point EvaluateCubicBezierCurveBernstein( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t );
Point EvaluateCubicBezierCurveMatrix( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t );
Point EvaluateCubicBezierCurveCasteljau( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t );


// Evaluate a cubic Hermite spline with control points 'controlPoints' arranged:
//     p0 derivative_at_p0 ( p1 derivative_at_p1 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
Points EvaluateCubicHermiteSpline( const Points& controlPoints, const int samplesPerCurve );
// Evaluate a cubic Hermite curve at location 't'.
Point EvaluateCubicHermiteCurve( const Point& p0, const Point& dp0, const Point& p1, const Point& dp1, const real_t t );

// Given a cubic Hermite spline with control points 'controlPoints' arranged:
//     p0 derivative_at_p0 ( p1 derivative_at_p1 )+
// replaces the derivative entries with values that result in a C2 continuous
// Hermite spline.
// NOTE: 'controlPoints' is an input and output parameter. The derivative entries are replaced.
void CalculateHermiteSplineDerivativesForC2Continuity( Points& controlPoints );


// Evaluate a Catmull-Rom Spline with control points 'controlPoints' arranged:
//     p0 p1 p2 ( p3 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
Points EvaluateCatmullRomSpline( const Points& controlPoints, const int samplesPerCurve, const real_t alpha );
// Evaluate a cubic Catmull-Rom Spline curve at location 't'.
Point EvaluateCatmullRomCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t, const real_t alpha );


// Evaluate a cubic B-Spline with control points 'controlPoints' arranged:
//     p0 p1 p2 ( p3 )+
// at positive integer 'samplesPerCurve' locations along each curve.
// Upon return, 'curvePointsOut' is cleared and replaced with the sampled points.
Points EvaluateCubicBSpline( const Points& controlPoints, const int samplesPerCurve );
// Evaluate a cubic B-Spline curve at location 't'.
Point EvaluateCubicBSplineCurve( const Point& p0, const Point& p1, const Point& p2, const Point& p3, const real_t t );

// Compute cubic BSpline control points that interpolate the given points.
Points ComputeBSplineFromInterpolatingPoints( const Points& interpPoints );
// Given a sequence of cubic BSpline control points, returns the interpolating points
// that could have been used to create them via ComputeBSplineFromInterpolatingPoints().
Points ComputeInterpolatingPointsFromBSpline( const Points& controlPoints );

}
