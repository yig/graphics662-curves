#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

#include <sstream>
#include <iomanip>

#include "Curve.h"

// Global functions for debugging.
// NOTE: You can pass either a const char* or an std::string.
namespace { pp::Instance* anyInstance; }
void jsAlert( const std::string& msg )
{
    if( anyInstance )
    {
        anyInstance->PostMessage( pp::Var( std::string( "alert " ) + msg ) );
    }
}
void jsLog( const std::string& msg )
{
    if( anyInstance )
    {
        anyInstance->PostMessage( pp::Var( std::string( "log " ) + msg ) );
    }
}

namespace
{

std::ostream& operator<<( std::ostream& out, const std::vector< Curve::Point >& pts )
{
    out << "[ ";
    for( unsigned int i = 0; i < pts.size(); ++i )
    {
        if( i > 0 ) out << ", ";
        
        out << "[ " << pts.at(i).x() << ", " << pts.at(i).y() << " ]";
    }
    out << " ]";
    
    return out;
}
std::istream& operator>>( std::istream& in, Curve::Point& pt )
{
    return in >> pt.x() >> pt.y();
}

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

class CurveInstance : public pp::Instance
{
public:
    explicit CurveInstance( PP_Instance instance ) : pp::Instance( instance ), m_curve(0)
    {
        // For debugging:
        anyInstance = this;
    }
    virtual ~CurveInstance() { delete m_curve; }
    
    void HandleMessage( const pp::Var& var_message )
    {
        // This slows everything down, but is useful for debugging:
        jsLog( std::string( "HandleMessage: " ) + var_message.AsString() );
        
        // We only expect string messages.
        if( !var_message.is_string() )
        {
            return;
        }
        
        // Turn the message into an istream for processing.
        std::istringstream msgstream( var_message.AsString() );
        
        std::string cmd;
        msgstream >> cmd;
        
        if( cmd == "AddPoint" )
        {
            Curve::Point p;
            msgstream >> p;
            if( m_curve ) m_curve->AddPoint( p );
        }
        else if( cmd == "SetControlPoint" )
        {
            int i;
            Curve::Point p;
            msgstream >> i >> p;
            if( m_curve ) m_curve->SetControlPoint( i, p );
        }
        else if( cmd == "ClearAll" )
        {
            delete m_curve;
            m_curve = NewCurveFactory( m_curveType );
        }
        else if( cmd == "SetCurveType" )
        {
            msgstream >> m_curveType;
            
            // Save the interpolated points before switching.
            std::vector< Curve::Point > interpolated;
            if( m_curve )
            {
                interpolated = m_curve->GetInterpolatedPoints();
            }
            
            // Switch to the new curve type.
            delete m_curve;
            m_curve = NewCurveFactory( m_curveType );
            
            // Restore the interpolated points after switching.
            if( m_curve )
            {
                for( int i = 0; i < interpolated.size(); ++i ) m_curve->AddPoint( interpolated.at(i) );
            }
        }
        else if( cmd == "GetData" )
        {
            std::vector< Curve::Point > controlPoints, curvePoints;
            
            if( m_curve )
            {
                controlPoints = m_curve->GetControlPoints();
                curvePoints = m_curve->GetCurvePoints();
            }
            
            // Package up some JSON and post it.
            std::ostringstream packet;
            // Set precision to 24 to preserve double-precision accuracy.
            packet << std::setprecision( 24 ) << std::boolalpha;
            packet << "{ \"controlPoints\": " << controlPoints;
            packet << ", \"curve\": " << curvePoints;
            packet << "}";
            
            PostMessage( pp::Var( std::string("GetData ") + packet.str() ) );
        }
        else
        {
            jsAlert( std::string( "Unknown command: " ) + var_message.AsString() );
        }
    }
    
private:
    std::string m_curveType;
    Curve::InterpolatingCurve* m_curve;
}; // ~CurveInstance

class CurveModule : public pp::Module
{
public:
    pp::Instance* CreateInstance( PP_Instance instance )
    {
        return new CurveInstance( instance );
    }
}; // ~CurveModule

} // ~anonymous

namespace pp
{
Module* CreateModule()
{
    return new CurveModule();
}
} // ~pp
