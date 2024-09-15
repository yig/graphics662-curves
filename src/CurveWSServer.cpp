#include "rtc/rtc.hpp"
// References I used:
// server onClient: <https://github.com/paullouisageneau/libdatachannel/blob/v0.21.2/include/rtc/websocketserver.hpp>
// server configuration: <https://github.com/paullouisageneau/libdatachannel/blob/v0.21.2/include/rtc/configuration.hpp>
// client send: <https://github.com/paullouisageneau/libdatachannel/blob/v0.21.2/include/rtc/websocket.hpp>
// client onMessage: <https://github.com/paullouisageneau/libdatachannel/blob/v0.21.2/include/rtc/channel.hpp> 
// string and binary data types: <https://github.com/paullouisageneau/libdatachannel/blob/v0.21.2/include/rtc/common.hpp>

#include <iostream>
#include <string>
#include <format>
#include <memory>
#include <future>

#include <chrono>
#include <thread>

// https://github.com/nlohmann/json
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "CurveManager.h"

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
    using namespace rtc;
    
    // Get the port as an optional command line argument.
    int port = 8000;
    if( argc == 3 && argv[1] == std::string("--port") ) port = std::stoi( argv[2] );
    
    // Make a WebSocket server.
    WebSocketServer server( WebSocketServerConfiguration{ .port = static_cast<uint16_t>( port ) } );
    
    // Announce it on the command line.
    std::cout << std::format( "Listening on ws://localhost:{}", port ) << std::endl;
    
    // Listen for clients.
    server.onClient( []( auto client ) {
        std::cout << "Client connected!" << std::endl;
        
        // auto manager = std::make_shared<Curve::CurveManager>();
        auto manager = new Curve::CurveManager();
        
        client->onMessage(
            // Capture `client` and `manager` by value, since they are `shared_ptr`s.
            [=]( auto binary ) {
                std::cout << std::format( "<Received {}-byte binary message.>", binary.size() ) << std::endl;
            },
            [=]( auto text ) {
                const json message = json::parse( text );
                std::cerr << "Message: " << text << std::endl;
                
                // If this line gives a weird error, change to `.get<std::string>()` or `.get_to()`.
                // See: <https://github.com/nlohmann/json/issues/958>
                const std::string command = message.at("command");
                // std::cerr << "Command: " << command << std::endl;
                
                if( command == "AddPoint" ) {
                    manager->AddPoint( message.at("point") );
                } else if( command == "SetControlPoint" ) {
                    manager->SetControlPoint( message.at("index"), message.at("point") );
                } else if( command == "ClearAll" ) {
                    manager->ClearAll();
                } else if( command == "SetCurveType" ) {
                    manager->SetCurveType( message.at("curveType") );
                } else if( command == "GetControlPoints" ) {
                    const json reply = manager->GetControlPoints();
                    // std::cerr << "Reply: " << reply.dump() << std::endl;
                    client->send( reply.dump() );
                } else if( command == "GetCurvePoints" ) {
                    const json reply = manager->GetCurvePoints();
                    // std::cerr << "Reply: " << reply.dump() << std::endl;
                    client->send( reply.dump() );
                } else if( command == "Ping" ) {
                    std::cout << "Pong" << std::endl;
                } else {
                    std::cerr << "Unknown command: " << command << std::endl;
                }
            }
        );
    } );
    
    // Sleep forever
    while( true ) { std::this_thread::sleep_for(std::chrono::hours(100000)); }
    
    return 0;
}
