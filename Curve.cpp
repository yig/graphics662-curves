#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

#include <sstream>
#include <iomanip>

#include "MyCurve.h"

namespace
{
// For debugging:
pp::Instance* anyInstance;

std::ostream& operator<<( std::ostream& out, const std::vector< MyCurve::Point >& pts )
{
    out << "[ ";
    for( unsigned int i = 0; i < pts.size(); ++i )
    {
        if( i > 0 ) out << ", ";
        
        out << "[ " << pts.at(i).x << ", " << pts.at(i).y << " ]";
    }
    out << " ]";
    
    return out;
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
        PostMessage( pp::Var( std::string( "log HandleMessage: " ) + var_message.AsString() ) );
        
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
            float x, y;
            msgstream >> x >> y;
            m_myCurve.AddPoint( x, y );
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
            vector<MyCurve::Point> endPoints, interpPoints, ctrlPoints, curve;
            
            m_myCurve.GetData( endPoints, interpPoints, ctrlPoints, curve );
            
            // Package up some JSON and post it.
            std::ostringstream packet;
            // Set precision to 24 to preserve double-precision accuracy.
            packet << std::setprecision( 24 );
            packet << "{ \"endPoints\": " << endPoints;
            packet << ", \"interpPoints\": " << interpPoints;
            packet << ", \"ctrlPoints\": " << ctrlPoints;
            packet << ", \"curve\": " << curve;
            packet << "}";
            
            PostMessage( pp::Var( std::string("GetData ") + packet.str() ) );
        }
        else
        {
            PostMessage( pp::Var( std::string( "alert Unknown command: " ) + var_message.AsString() ) );
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

// Global functions for debugging.
void jsAlert( const char* msg )
{
    if( anyInstance )
    {
        anyInstance->PostMessage( pp::Var( std::string( "alert " ) + msg ) );
    }
}
void jsLog( const char* msg )
{
    if( anyInstance )
    {
        anyInstance->PostMessage( pp::Var( std::string( "log " ) + msg ) );
    }
}

namespace pp
{
Module* CreateModule()
{
    return new CurveModule();
}
} // ~pp
