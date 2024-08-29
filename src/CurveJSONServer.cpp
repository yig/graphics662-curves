#include "CurveFunctions.h"
#include "CurveManager.h"

#include <iostream>
#include <string>

// #include "spdlog/spdlog.h"

// https://github.com/nlohmann/json
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// The `to_json()` and `from_json()` functions must be in the same namespace as Point, which is actually an Eigen type.
namespace Curve {

// A magic macro! But we want points to get sent as an array of two items.
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( Curve::CurveManager::CurveManagerPoint, x, y )

void to_json( json& j, const Curve::CurveManager::CurveManagerPoint& p ) {
    j = { p.x, p.y };
}

void from_json( const json& j, Curve::CurveManager::CurveManagerPoint& p ) {
    j.at(0).get_to( p.x );
    j.at(1).get_to( p.y );
}

}

int main( int argc, char* argv[] ) {
    
    Curve::CurveManager manager;
    
    while( true ) {
        std::string line;
        std::getline( std::cin, line );
        
        const json message = json::parse( line );
        
        // If this line gives a weird error, change to `.get<std::string>()` or `.get_to()`.
        // See: <https://github.com/nlohmann/json/issues/958>
        const std::string command = message.at("command");
        
        if( command == "AddPoint" ) {
            manager.AddPoint( message.at("point") );
        } else if( command == "SetControlPoint" ) {
            manager.SetControlPoint( message.at("index"), message.at("point") );
        } else if( command == "ClearAll" ) {
            manager.ClearAll();
        } else if( command == "SetCurveType" ) {
            manager.SetCurveType( message.at("curveType") );
        } else if( command == "GetControlPoints" ) {
            const json reply = manager.GetControlPoints();
            // Use `endl` to flush.
            std::cout << reply << std::endl;
        } else if( command == "GetCurvePoints" ) {
            const json reply = manager.GetCurvePoints();
            // Use `endl` to flush.
            std::cout << reply << std::endl;
        } else if( command == "Ping" ) {
            std::cout << "Pong" << std::endl;
        }
    }
    
    return 0;
}
