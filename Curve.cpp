#include "Curve.h"

#include <cassert>
#include <cmath>

#include <Eigen/Core>
#include <Eigen/LU>
using Eigen::MatrixXd;
using std::vector;

// Call these to raise a dialog box or log to the javascript console for debugging.
// NOTE: You can pass either a const char* or an std::string.
extern void jsAlert( const std::string& msg );
extern void jsLog( const std::string& msg );

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
InterpolatingCurve::GetCurve()
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
        m_controlPoints.push_back( (1./3.)*( m_controlPoints.back() + p ) );
        m_controlPoints.push_back( (2./3.)*( m_controlPoints.back() + p ) );
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
        m_controlPoints.push_back( (2./3.)*( m_controlPoints.back() + p ) );
        
        // 3 Add the point.
        m_controlPoints.push_back( p );
    }
}

// Override doSetControlPoint() in order to keep C1 continuity
// when a non-interpolated control point is moved.
void
CubicBezierCurve::doSetControlPoint( int i, const Point& p )
{
    m_controlPoints.at( i ) = p;
    
    /// Cubic Bezier splines always have 3*n + 1 control points.
    /// The 0-th, 3-rd, 6-th, 9-th, etc points are interpolated.
    /// The rest are tangent points.
    /// For C1 continuity, the 2nd and 4th should be reflected about the 3rd,
    /// the 5th and 7th should be reflected about the 6th,
    /// and so on in offsets of 3.
    
    // Check for the 2nd and so on case.
    if( i >= 2 && (i-2) % 3 == 0 )
    {
        const Point& on_curve = m_controlPoints.at( i+1 );
        m_controlPoints.at( i+2 ) = on_curve + (on_curve - p);
    }
    // Check for the 4th and so on case.
    else if( i >= 4 && (i-4) % 3 == 0 )
    {
        const Point& on_curve = m_controlPoints.at( i-1 );
        m_controlPoints.at( i-2 ) = on_curve + (on_curve - p);
    }
    // Check for the 5th and so on case.
    else if( i >= 5 && (i-5) % 3 == 0 )
    {
        const Point& on_curve = m_controlPoints.at( i+1 );
        m_controlPoints.at( i+2 ) = on_curve + (on_curve - p);
    }
    // Check for the 7th and so on case.
    else if( i >= 7 && (i-7) % 3 == 0 )
    {
        const Point& on_curve = m_controlPoints.at( i-1 );
        m_controlPoints.at( i-2 ) = on_curve + (on_curve - p);
    }
}

// Evaluated the given control points to fill m_curvePoints.
void CubicBezierCurveBernstein::doEvaluate() const
{
    assert( m_curvePoints.empty() );
    EvaluateCubicBezierSplineBernstein( m_controlPoints, kSamplesPerCurve, m_curvePoints );
}
void CubicBezierCurveMatrix::doEvaluate() const
{
    assert( m_curvePoints.empty() );
    EvaluateCubicBezierSplineMatrix( m_controlPoints, kSamplesPerCurve, m_curvePoints );
}
void CubicBezierCurveCasteljau::doEvaluate() const
{
    assert( m_curvePoints.empty() );
    EvaluateCubicBezierSplineCasteljau( m_controlPoints, kSamplesPerCurve, m_curvePoints );
}

/// ======================================================================================

// When adding a point, add new non-interpolated control points.
void
CubicHermiteCurve::doAddPoint( const Point& p )
{
    // Give new points (0,0) derivatives.
    m_controlPoints.push_back( p );
    m_controlPoints.push_back( Point( 0,0 ) );
    
    // Recompute derivatives for C2 continuity.
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
    EvaluateCubicHermiteSpline( m_controlPoints, kSamplesPerCurve, m_curvePoints );
}

/// ======================================================================================

// Add a point.
void
CubicCatmullRomCurve::doAddPoint( const Point& p )
{
    m_controlPoints.push_back( p );
}

// Evaluated the given control points to fill m_curvePoints.
void
CubicCatmullRomCurve::doEvaluate()
const
{
    assert( m_curvePoints.empty() );
    EvaluateCatmullRomSpline( m_controlPoints, kSamplesPerCurve, m_curvePoints );
}

/// ======================================================================================

// Add a point.
void
CubicCatmullRomCurve::doAddPoint( const Point& p )
{
    m_controlPoints.push_back( p );
}

// Evaluated the given control points to fill m_curvePoints.
void
CubicCatmullRomCurve::doEvaluate()
const
{
    assert( m_curvePoints.empty() );
    EvaluateCatmullRomSpline( m_controlPoints, kSamplesPerCurve, m_curvePoints );
}

}
