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

std::ostream& operator<<( std::ostream& out, const std::vector< MyCurve::Point >& pts )
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
std::istream& operator>>( std::istream& in, MyCurve::Point& pt )
{
    return in >> pt.x() >> pt.y();
}

class CurveInstance : public pp::Instance
{
public:
    explicit CurveInstance( PP_Instance instance ) : pp::Instance( instance )
    {
        // For debugging:
        anyInstance = this;
    }
    virtual ~CurveInstance() {}
    
    virtual void HandleMessage( const pp::Var& var_message )
    {
        // This slows everything down, but is useful for debugging:
        jsLog( std::string( "log HandleMessage: " ) + var_message.AsString() );
        
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
            MyCurve::Point p;
            msgstream >> p;
            m_myCurve.AddPoint( p );
        }
        else if( cmd == "PickPoint" )
        {
            float x, y;
            msgstream >> x >> y;
            m_myCurve.PickPoint( x, y );
        }
        else if( cmd == "MovePicked" )
        {
            float x, y;
            msgstream >> x >> y;
            m_myCurve.MovePicked( x, y );
        }
        else if( cmd == "ClearAll" )
        {
            m_myCurve.ClearAll();
        }
        else if( cmd == "SetShowControlPoints" )
        {
            bool ctrlPoints;
            msgstream >> std::boolalpha >> ctrlPoints;
            
            m_myCurve.SetShowControlPoints( ctrlPoints );
        }
        else if( cmd == "SetInterpolationStyle" )
        {
            std::string stylestr;
            msgstream >> stylestr;
            
            MyCurve::MyCurve::InterpolationStyle style = MyCurve::MyCurve::INVALID_STYLE;
            
            if( stylestr == "BERNSTEIN" ) style = MyCurve::MyCurve::BERNSTEIN;
            else if( stylestr == "CASTELJAU" ) style = MyCurve::MyCurve::CASTELJAU;
            else if( stylestr == "MATRIX" ) style = MyCurve::MyCurve::MATRIX;
            else if( stylestr == "BSPLINE" ) style = MyCurve::MyCurve::BSPLINE;
            else if( stylestr == "HERMITE" ) style = MyCurve::MyCurve::HERMITE;
            
            m_myCurve.SetInterpolationStyle( style );
        }
        else if( cmd == "GetData" )
        {
            std::vector<MyCurve::Point> endPoints, interpPoints, ctrlPoints, curve;
            
            m_myCurve.GetData( endPoints, interpPoints, ctrlPoints, curve );
            
            // Package up some JSON and post it.
            std::ostringstream packet;
            // Set precision to 24 to preserve double-precision accuracy.
            packet << std::setprecision( 24 ) << std::boolalpha;
            packet << "{ \"endPoints\": " << endPoints;
            packet << ", \"interpPoints\": " << interpPoints;
            packet << ", \"ctrlPoints\": " << ctrlPoints;
            packet << ", \"curve\": " << curve;
            packet << "}";
            
            PostMessage( pp::Var( std::string("GetData ") + packet.str() ) );
        }
        else
        {
            jsAlert( std::string( "alert Unknown command: " ) + var_message.AsString() );
        }
    }
    
private:
    MyCurve::MyCurve m_myCurve;
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
