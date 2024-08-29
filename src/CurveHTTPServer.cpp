#include "CurveFunctions.h"

#include <iostream>
#include <string>
#include <format>

// #include "spdlog/spdlog.h"

// https://github.com/nlohmann/json
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// https://github.com/yhirose/cpp-httplib
#include <httplib.h>

// The `to_json()` and `from_json()` functions must be in the same namespace as Point, which is actually an Eigen type.
namespace Eigen {

// A magic macro! But it doesn't work for some reason.
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( Curve::Point, x(), y() )

void to_json( json& j, const Curve::Point& p ) {
    j = { p(0), p(1) };
}

void from_json( const json& j, Curve::Point& p ) {
    j.at(0).get_to( p(0) );
    j.at(1).get_to( p(1) );
}

}

int main( int argc, char* argv[] ) {
    int port = 8000;
    if( argc == 3 && argv[1] == std::string("--port") ) port = std::stoi( argv[2] );
    
    // Make an HTTP server.
    httplib::Server server;
    
    // Log everything
    server.set_logger( []( const auto& request, const auto& response ) {
        std::cout << std::format( "=====> {} {} {}\n", request.method, request.version, request.path );
        
        std::cout << std::format( "parameters:\n" );
        for( auto it = request.params.begin(); it != request.params.end(); ++it ) {
            std::cout << std::format( "\t {}: {}\n", it->first, it->second );
        }
        
        std::cout << std::format( "headers:\n" );
        for( const auto& h : request.headers ) { std::cout << std::format( "\t{}: {}\n", h.first, h.second ); }
        
        std::cout << std::format( "body: {}\n", request.body );
        
        std::cout << std::format( "files:\n" );
        for( const auto& f : request.files ) {
            std::cout << std::format( "\tname: {}\n", f.first );
            std::cout << std::format( "\tfilename: {}\n", f.second.filename );
            std::cout << std::format( "\tcontent type: {}\n", f.second.content_type );
            std::cout << std::format( "\tsize: {}\n", f.second.content.size() );
            std::cout << std::format( "\t---\n" );
        }
        
        std::cout << "------------------------------------------------------\n";
        
        std::cout << std::format( "response status: {}\n", response.status );
        std::cout << std::format( "response headers:\n" );
        for( const auto& h : response.headers ) { std::cout << std::format( "\t{}: {}\n", h.first, h.second ); }
        // This appears to be gzipped.
        // std::cout << std::format( "response body: {}\n", response.body );
        
        std::cout << '\n';
    } );
    
    // Serve files next to the binary
    server.set_mount_point( "/static", "./" );
    
    std::cout << std::format( "Open http://localhost:{}/static/Curve.html", port ) << '\n';
    
    // Listen for calls to GetCurvePoints.
    server.Post("/GetCurvePoints", [&]( const auto& request, auto& response ) {
        const json body = json::parse( request.body );
        
        // If this line gives a weird error, change to `.get<std::string>()` or `.get_to()`.
        // See: <https://github.com/nlohmann/json/issues/958>
        const std::string approach = body.at( "Approach" );
        Curve::Points ControlPoints;
        body.at( "ControlPoints" ).get_to( ControlPoints );
        
        int SamplesPerCurve = 20;
        if( body.contains( "SamplesPerCurve" ) ) {
            body.at("SamplesPerCurve").get_to( SamplesPerCurve );
        }
        
        // For Catmull-Rom Splines
        double alpha = 0.5;
        if( body.contains( "Alpha" ) ) {
            body.at("Alpha").get_to( alpha );
        }
        
        Curve::Points result;
        if( ControlPoints.size() < 4 ) {
            // Do nothing. Return an empty result. We don't have enough control points.
        } else if( approach == "CubicBezierBernstein" ) {
            result = Curve::EvaluateCubicBezierSpline( ControlPoints, SamplesPerCurve, Curve::EvaluateCubicBezierCurveApproach::Bernstein );
        } else if( approach == "CubicBezierMatrix" ) {
            result = Curve::EvaluateCubicBezierSpline( ControlPoints, SamplesPerCurve, Curve::EvaluateCubicBezierCurveApproach::Matrix );
        } else if( approach == "CubicBezierCasteljau" ) {
            result = Curve::EvaluateCubicBezierSpline( ControlPoints, SamplesPerCurve, Curve::EvaluateCubicBezierCurveApproach::Matrix );
        } else if( approach == "CubicHermite" ) {
            result = Curve::EvaluateCubicHermiteSpline( ControlPoints, SamplesPerCurve );
        } else if( approach == "CatmullRom" ) {
            result = Curve::EvaluateCatmullRomSpline( ControlPoints, SamplesPerCurve, alpha );
        } else if( approach == "CubicBSpline" ) {
            result = Curve::EvaluateCubicBSpline( ControlPoints, SamplesPerCurve );
        } else {
            std::cout << std::format( R"(
            std::cerr << "ERROR: Unknown approach: " << approach << std::endl;
            )" ) << '\n';
            response.status = httplib::BadRequest_400;
            response.set_content( std::format( "Unknown approach: {}", approach ), "text/plain" );
            return;
        }
        
        response.set_content( json( result ).dump(), "application/json" );
        });
    
    // Listen for calls to GetCurvePoints.
    server.Post("/Ping", [&]( const auto& request, auto& response ) {
        response.set_content( "Pong", "text/plain" );
        // spdlog::info( "Ping." );
        });
    
    server.listen( "0.0.0.0", port );
    
    return 0;
}
