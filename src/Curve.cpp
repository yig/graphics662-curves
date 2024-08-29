#include "Curve.h"
using std::vector;

#include <cassert>
#include <cmath>

// #include "jsassert.h"
// #undef assert
// #define assert(cond) jsAssert(cond)

// Call these to raise a dialog box or log to the javascript console for debugging.
// NOTE: You can pass either a const char* or an std::string.
// void jsAlert( const std::string& msg );
// void jsLog( const std::string& msg );
// void jsWarn( const std::string& msg );
// void jsError( const std::string& msg );

namespace
{
    const int kSamplesPerCurve = 20;
}

namespace Curve
{

// Add another point to the sequence of input points.
void
InterpolatingCurve::AddPoint( const Point& p )
{
    doAddPoint( p );
    NeedEvaluate();
}

// Get the control points for this spline.
// Note that the format of the control points can vary (such as Hermite splines storing derivatives).
const std::vector< Point >&
InterpolatingCurve::GetControlPoints()
const
{
    return m_controlPoints;
}

// Sets the control point at index 'i' to 'p'.
void
InterpolatingCurve::SetControlPoint( int i, const Point& p )
{
    doSetControlPoint( i, p );
    NeedEvaluate();
}

// Returns points sampling the spline curve defined by the control points.
const std::vector< Point >&
InterpolatingCurve::GetCurvePoints()
const
{
    if( m_curvePoints.empty() ) doEvaluate();
    
    return m_curvePoints;
}

void
InterpolatingCurve::NeedEvaluate()
{
    m_curvePoints.clear();
}

/// ======================================================================================

void
CubicBezierCurve::SetEvaluateApproach( EvaluateCubicBezierCurveApproach approach )
{
    m_approach = approach;
    NeedEvaluate();
}

std::vector< Point >
CubicBezierCurve::GetInterpolatedPoints() const
{
    std::vector< Point > result;
    // The 0th point, 3rd point, 6th point, etc are all interpolated.
    for( int i = 0; i < m_controlPoints.size(); i += 3 )
    {
        result.push_back( m_controlPoints.at(i) );
    }
    return result;
}

// When adding a point, add new non-interpolated control points.
void
CubicBezierCurve::doAddPoint( const Point& p )
{
    // If this is our first point, just add it.
    if( m_controlPoints.empty() )
    {
        m_controlPoints.push_back( p );
    }
    // If this is our second point, 
    else if( m_controlPoints.size() == 1 )
    {
        m_controlPoints.push_back( (2./3.)*m_controlPoints.back() + (1./3.)*p );
        m_controlPoints.push_back( (1./3.)*m_controlPoints.back() + (2./3.)*p );
        m_controlPoints.push_back( p );
    }
    // Otherwise we have the general case, where we want to reflect the previous point's tangent.
    else
    {
        // 1 Reflect the previous point's tangent.
        const Point& last_point = m_controlPoints.back();
        const Point& last_off_curve = *(m_controlPoints.rbegin()+1);
        m_controlPoints.push_back( last_point + (last_point - last_off_curve) );
        
        // 2 Add a new tangent.
        m_controlPoints.push_back( (1./3.)*m_controlPoints.back() + (2./3.)*p );
        
        // 3 Add the point.
        m_controlPoints.push_back( p );
    }
}

// Override doSetControlPoint() in order to keep C1 continuity
// when a non-interpolated control point is moved.
void
CubicBezierCurve::doSetControlPoint( int i, const Point& p )
{
    // Make an alias to the control points for shorter expressions.
    vector< Point >& C = m_controlPoints;
    
    /// Cubic Bezier splines always have 3*n + 1 control points.
    /// The 0-th, 3-rd, 6-th, 9-th, etc points are interpolated.
    /// The rest are tangent points.
    /// For C1 continuity, the 2nd and 4th should be reflected about the 3rd,
    /// the 5th and 7th should be reflected about the 6th,
    /// and so on in offsets of 3.
    
    // Check for the 2nd and so on case.
    if( i >= 2 && (i-2) % 3 == 0 )
    {
        const Point& on_curve = C.at( i+1 );
        if( i+2 < C.size() )
        {
            C.at( i+2 ) = on_curve + (on_curve - p);
        }
    }
    // Check for the 4th and so on case.
    else if( i >= 4 && (i-4) % 3 == 0 )
    {
        const Point& on_curve = C.at( i-1 );
        if( i-2 >= 0 )
        {
            C.at( i-2 ) = on_curve + (on_curve - p);
        }
    }
    // Check for the 5th and so on case.
    else if( i >= 5 && (i-5) % 3 == 0 )
    {
        const Point& on_curve = C.at( i+1 );
        if( i+2 < C.size() )
        {
            C.at( i+2 ) = on_curve + (on_curve - p);
        }
    }
    // Check for the 7th and so on case.
    else if( i >= 7 && (i-7) % 3 == 0 )
    {
        const Point& on_curve = C.at( i-1 );
        if( i-2 >= 0 )
        {
            C.at( i-2 ) = on_curve + (on_curve - p);
        }
    }
    // Finally, if we are moving an on-curve control point itself,
    // translate the adjacent tangents the same way.
    else if( i % 3 == 0 )
    {
        const Point& dp = p - C.at( i );
        
        if( i-1 >= 0 )
        {
            C.at( i-1 ) += dp;
        }
        if( i+1 < C.size() )
        {
            C.at( i+1 ) += dp;
        }
    }
    
    C.at( i ) = p;
}

// Evaluated the given control points to fill m_curvePoints.
void
CubicBezierCurve::doEvaluate()
const
{
    assert( m_curvePoints.empty() );
    // We can't evaluate if we don't have at least 4 points.
    if( m_controlPoints.size() < 4 ) return;
    m_curvePoints = EvaluateCubicBezierSpline( m_controlPoints, kSamplesPerCurve, m_approach );
}

/// ======================================================================================

std::vector< Point >
CubicHermiteCurve::GetInterpolatedPoints() const
{
    std::vector< Point > result;
    // The even points are interpolated.
    for( int i = 0; i < m_controlPoints.size(); i += 2 )
    {
        result.push_back( m_controlPoints.at(i) );
    }
    return result;
}

// When adding a point, add new non-interpolated control points.
void
CubicHermiteCurve::doAddPoint( const Point& p )
{
    // Give new points (0,0) derivatives.
    m_controlPoints.push_back( p );
    m_controlPoints.push_back( Point( 0,0 ) );
    // If we have more than one interpolated point, set the new derivative
    // to 1/2 of the vector between them.
    if( m_controlPoints.size() > 4 )
    {
        m_controlPoints.back() = .5*( m_controlPoints.at( m_controlPoints.size()-2 ) - m_controlPoints.at( m_controlPoints.size()-4 ) );
    }
    
    // Compute derivatives to ensure C2 continuity.
    CalculateHermiteSplineDerivativesForC2Continuity( m_controlPoints );
}

// Override doSetControlPoint() in order to keep C2 continuity
// when a non-derivative control point is moved.
void
CubicHermiteCurve::doSetControlPoint( int i, const Point& p )
{
    m_controlPoints.at( i ) = p;
    
    // After adjusting a control point, we must recompute the derivatives
    // to ensure that the curve stays C2 continuous.
    // NOTE: This means that the derivatives will never be adjustable,
    //       unless the bonus is implemented which allows the derivatives at
    //       either end to be adjusted.
    CalculateHermiteSplineDerivativesForC2Continuity( m_controlPoints );
}

// Evaluated the given control points to fill m_curvePoints.
void
CubicHermiteCurve::doEvaluate()
const
{
    assert( m_curvePoints.empty() );
    // We can't evaluate if we don't have at least 4 points.
    if( m_controlPoints.size() < 4 ) return;
    m_curvePoints = EvaluateCubicHermiteSpline( m_controlPoints, kSamplesPerCurve );
}

/// ======================================================================================

std::vector< Point >
CatmullRomCurve::GetInterpolatedPoints() const
{
    // Every point is interpolated.
    return m_controlPoints;
}

// Add a point.
void
CatmullRomCurve::doAddPoint( const Point& p )
{
    m_controlPoints.push_back( p );
}

// Evaluated the given control points to fill m_curvePoints.
void
CatmullRomCurve::doEvaluate()
const
{
    assert( m_curvePoints.empty() );
    // We can't evaluate if we don't have at least 4 points.
    if( m_controlPoints.size() < 4 ) return;
    m_curvePoints = EvaluateCatmullRomSpline( m_controlPoints, kSamplesPerCurve, m_alpha );
}

/// ======================================================================================

std::vector< Point >
CubicBSplineCurve::GetInterpolatedPoints() const
{
    return ComputeInterpolatingPointsFromBSpline( m_controlPoints );
}

// Add a point.
void
CubicBSplineCurve::doAddPoint( const Point& p )
{
    // Until we have two points, store the points directly in m_controlPoints.
    if( m_controlPoints.empty() ) m_controlPoints.push_back( p );
    // If we already have one, compute a BSpline to interpolate these two.
    else if( m_controlPoints.size() == 1 )
    {
        std::vector< Point > interpPoints( m_controlPoints );
        interpPoints.push_back( p );
        m_controlPoints.clear();
        
        // Compute the BSpline interpolating the points.
        m_controlPoints = ComputeBSplineFromInterpolatingPoints( interpPoints );
    }
    // If we already have more than one, compute the previously interpolated points
    // and then recreate the BSpline.
    else
    {
        // Figure out the interpolating points created from the BSpline.
        std::vector< Point > interpPoints = ComputeInterpolatingPointsFromBSpline( m_controlPoints );
        // Add the new one to the end.
        interpPoints.push_back( p );
        // Compute the BSpline interpolating the points.
        m_controlPoints = ComputeBSplineFromInterpolatingPoints( interpPoints );
    }
}

// Evaluated the given control points to fill m_curvePoints.
void
CubicBSplineCurve::doEvaluate()
const
{
    assert( m_curvePoints.empty() );
    if( m_controlPoints.size() < 4 ) return;
    m_curvePoints = EvaluateCubicBSpline( m_controlPoints, kSamplesPerCurve );
}

}
