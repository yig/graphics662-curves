#ifndef __Curve_h__
#define __Curve_h__

#include "CurveFunctions.h"

#include <vector>
#include <Eigen/Core>

namespace Curve
{

/*
 *	The InterpolatingCurve class implements various splines given interpolating points.
 */
class InterpolatingCurve
{
public:
    InterpolatingCurve() {}
    virtual ~Curve() {}
    
    // Add another point to the sequence of input points.
    void AddPoint( const Point& p );
    
    // Get the control points for this spline.
    // Note that the format of the control points can vary (such as Hermite splines storing derivatives).
    const std::vector< Point >& GetControlPoints() const;
    
    // Sets the control point at index 'i' to 'p'.
    void SetControlPoint( int i, const Point& p );
    
    // Returns points sampling the spline curve defined by the control points.
    const std::vector< Point >& GetCurvePoints() const;

protected:
    /// For subclasses to override.
    // Add this interpolated point to the curve.
    virtual void doAddPoint( const Point& p ) = 0;
    
    // Evaluated the given control points to fill m_curvePoints.
    virtual void doEvaluate() = 0;
    
    // Set the given control point. Update the others if needed.
    virtual void doSetControlPoint( int i, const Point& p )
    {
        m_controlPoints.at(i) = p;
    }
    
    std::vector< Point > m_controlPoints;
    mutable std::vector< Point > m_curvePoints;

private:
    void NeedEvaluate();
};

// A class implementing everything for Bezier curves except Evaluate();
class CubicBezierCurve : public InterpolatingCurve
{
protected:
    // When adding a point, add new non-interpolated control points.
    void doAddPoint( const Point& p );
    // Override doSetControlPoint() in order to keep C1 continuity
    // when a non-interpolated control point is moved.
    void doSetControlPoint( int i, const Point& p );
    // We have three strategies for evaluating a Bezier curve.
    virtual void doEvaluate() const = 0;
};

// Classes implementing three different approaches to evaluation.
class CubicBezierCurveBernstein : public CubicBezierCurve
{
protected:
    void doEvaluate() const;
};
class CubicBezierCurveMatrix : public BezierCurve
{
protected:
    void doEvaluate() const;
};
class CubicBezierCurveCasteljau : public BezierCurve
{
protected:
    void doEvaluate() const;
};

// A class implementing everything for Hermite curves with C2 continuity of the added points.
class HermiteCurve : public Curve
{
protected:
    // When adding a point, add initial derivatives, too.
    void doAddPoint( const Point& p );
    // When setting an interpolated control point, call CalculateDerivativesForC2Continuity().
    void doSetControlPoint( int i, const Point& p );
    // Evaluated the given control points to fill m_curvePoints.
    void doEvaluate() const;
};

// A class implementing Catmull-Rom splines.
class CatmullRomCurve : public Curve
{
public:
    CatmullRomCurve( real_t alpha ) : m_alpha( alpha ) {}

protected:
    // When adding a point, add initial derivatives, too.
    void doAddPoint( const Point& p );
    // Evaluated the given control points to fill m_curvePoints.
    void doEvaluate() const;
};

// A class implementing everything for BSpline curves that interpolate the added points.
class BSplineCurve : public Curve
{
protected:
    // When adding a point, add initial derivatives, too.
    void doAddPoint( const Point& p );
    // Evaluated the given control points to fill m_curvePoints.
    void doEvaluate() const;
    // Given a sequence of interpolated points, calculate 
    void CalculateControlPointsFromInterpolatedPoints();
    
    // TODO Q: Do I want this, or do I want to extract the previous
    //         interpolated points from an on-the-fly evaluation of the curve?
    std::vector< Point > m_interpolatedPoints;
};

#endif /* __Curve_h__ */
