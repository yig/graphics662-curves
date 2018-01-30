#include <sstream>
#include <iomanip>

#include "Curve.h"
#include "jsassert.h"

#include <emscripten.h>
#include <emscripten/bind.h>
#include <memory> // std::unique_ptr

// Returns a new Curve::InterpolatingCurve* based on the string.
// If no such class is known, returns 0.
Curve::InterpolatingCurve* NewCurveFactory( const std::string& curveType )
{
    if( curveType == "CubicBezierBernstein" ) return new Curve::CubicBezierCurve( Curve::BernsteinApproach );
    else if( curveType == "CubicBezierCasteljau" ) return new Curve::CubicBezierCurve( Curve::CasteljauApproach );
    else if( curveType == "CubicBezierMatrix" ) return new Curve::CubicBezierCurve( Curve::MatrixApproach );
    else if( curveType == "CubicHermite" ) return new Curve::CubicHermiteCurve();
    else if( curveType == "CatmullRom" ) return new Curve::CatmullRomCurve(.5);
    else if( curveType == "CubicBSpline" ) return new Curve::CubicBSplineCurve();
    else {
        jsAlert( "Unknown curve type: " + curveType );
        return 0;
    }
}

namespace Curve
{

class CurveManager
{
public:
    struct CurveManagerPoint
    {
        CurveManagerPoint() : x(0), y(0) {}
        CurveManagerPoint( real_t a_x, real_t a_y ) : x( a_x ), y( a_y ) {}
        
        real_t x;
        real_t y;
    };
    
    void AddPoint( CurveManagerPoint p )
    {
        if( m_curve ) m_curve->AddPoint( Curve::Point( p.x, p.y ) );
    }
    void SetControlPoint( int i, CurveManagerPoint p )
    {
        if( m_curve ) m_curve->SetControlPoint( i, Curve::Point( p.x, p.y ) );
    }
    void ClearAll()
    {
        m_curve.reset( NewCurveFactory( m_curveType ) );
    }
    void SetCurveType( const std::string& curveType )
    {
        m_curveType = curveType;
        
        // Save the interpolated points before switching.
        std::vector< Curve::Point > interpolated;
        if( m_curve )
        {
            interpolated = m_curve->GetInterpolatedPoints();
        }
        
        // Switch to the new curve type.
        m_curve.reset( NewCurveFactory( m_curveType ) );
        
        // Restore the interpolated points after switching.
        if( m_curve )
        {
            for( unsigned int i = 0; i < interpolated.size(); ++i ) m_curve->AddPoint( interpolated.at(i) );
        }
    }
    std::vector< CurveManagerPoint > GetControlPoints()
    {
        std::vector< CurveManagerPoint > result;
        if( m_curve ) {
            std::vector< Curve::Point > points = m_curve->GetControlPoints();
            result.clear();
            result.reserve( points.size() );
            for( const auto& p : points ) { result.emplace_back( p(0), p(1) ); }
        }
        return result;
    }
    std::vector< CurveManagerPoint > GetCurvePoints()
    {
        std::vector< CurveManagerPoint > result;
        if( m_curve ) {
            std::vector< Curve::Point > points = m_curve->GetCurvePoints();
            result.clear();
            result.reserve( points.size() );
            for( const auto& p : points ) { result.emplace_back( p(0), p(1) ); }
        }
        return result;
    }
    
private:
    std::string m_curveType;
    std::unique_ptr< Curve::InterpolatingCurve > m_curve;
}; // ~CurveManager

}


EMSCRIPTEN_BINDINGS(Curve) {
    using namespace emscripten;
    
    value_array<Curve::CurveManager::CurveManagerPoint>("Point")
        .element(&Curve::CurveManager::CurveManagerPoint::x)
        .element(&Curve::CurveManager::CurveManagerPoint::y)
        ;
    
    register_vector<Curve::CurveManager::CurveManagerPoint>("VectorPoint");
    
    class_<Curve::CurveManager>("CurveManager")
        .constructor()
        .function("AddPoint", &Curve::CurveManager::AddPoint)
        .function("SetControlPoint", &Curve::CurveManager::SetControlPoint)
        .function("ClearAll", &Curve::CurveManager::ClearAll)
        .function("SetCurveType", &Curve::CurveManager::SetCurveType)
        .function("GetControlPoints", &Curve::CurveManager::GetControlPoints)
        .function("GetCurvePoints", &Curve::CurveManager::GetCurvePoints)
        ;
}
