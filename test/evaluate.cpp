#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "PrintToInclude.h"

// Run once with GENERATE_DATA defined, then copy the resulting .h files into `test/`.
// #define GENERATE_DATA
#ifdef GENERATE_DATA
    #include "GetBootstrapped.h"
#else
    #include "GetEvaluateCubicBezierSplineBernstein.h"
    #include "GetEvaluateCubicBezierSplineMatrix.h"
    #include "GetEvaluateCubicBezierSplineCasteljau.h"
    #include "GetEvaluateCubicHermiteSpline.h"
    #include "GetEvaluateCatmullRomSpline.h"
    #include "GetEvaluateCubicBSpline.h"
#endif

#include "CurveFunctions.h"
#include <algorithm>

using namespace Curve;

namespace
{
const Points ControlPoints = { { 403, 74 },
    { 242, 186 },
    { 217, 321 },
    { 323, 402 },
    { 459, 335 },
    { 430, 231 },
    { 334, 231 },
    { 270, 274 },
    };
const int SamplesPerCurve = 31;

typedef double real;
const real eps = 1e-5;

const std::vector< std::tuple< std::string, std::function<Points(const Points& pts, int SamplesPerCurve)>, std::function<Points()> > > functions{
    { "EvaluateCubicBezierSplineBernstein", []( auto pts, auto s ) { return EvaluateCubicBezierSpline( pts, s, EvaluateCubicBezierCurveApproach::Bernstein ); }, GetEvaluateCubicBezierSplineBernstein },
    { "EvaluateCubicBezierSplineMatrix", []( auto pts, auto s ) { return EvaluateCubicBezierSpline( pts, s, EvaluateCubicBezierCurveApproach::Matrix ); }, GetEvaluateCubicBezierSplineMatrix },
    { "EvaluateCubicBezierSplineCasteljau", []( auto pts, auto s ) { return EvaluateCubicBezierSpline( pts, s, EvaluateCubicBezierCurveApproach::Casteljau ); }, GetEvaluateCubicBezierSplineCasteljau },
    
    { "EvaluateCubicHermiteSpline", EvaluateCubicHermiteSpline, GetEvaluateCubicHermiteSpline },
    
    { "EvaluateCatmullRomSpline", []( auto pts, auto s ) { return EvaluateCatmullRomSpline( pts, s, 0.5 ); }, GetEvaluateCatmullRomSpline },
    
    { "EvaluateCubicBSpline", EvaluateCubicBSpline, GetEvaluateCubicBSpline },
};

}

/*
        int SamplesPerCurve = 20;
        if( request.has_param( "SamplesPerCurve" ) ) {
            SamplesPerCurve = std::stoi( request.get_param_value( "SamplesPerCurve" ) );
        }
        
        // For Catmull-Rom Splines
        double alpha = 0.5;
        if( request.has_param( "Alpha" ) ) {
            alpha = std::stod( request.get_param_value( "Alpha" ) );
        }
        
        Curve::Points result;
        if( approach == "EvaluateCubicBezierSplineBernstein" ) {
            result = Curve::EvaluateCubicBezierSpline( ControlPoints, SamplesPerCurve, Curve::EvaluateCubicBezierCurveApproach::Bernstein );
        } else if( approach == "EvaluateCubicBezierSplineMatrix" ) {
            result = Curve::EvaluateCubicBezierSpline( ControlPoints, SamplesPerCurve, Curve::EvaluateCubicBezierCurveApproach::Matrix );
        } else if( approach == "EvaluateCubicBezierSplineCasteljau" ) {
            result = Curve::EvaluateCubicBezierSpline( ControlPoints, SamplesPerCurve, Curve::EvaluateCubicBezierCurveApproach::Matrix );
        } else if( approach == "EvaluateCubicHermiteSpline" ) {
            result = Curve::EvaluateCubicHermiteSpline( ControlPoints, SamplesPerCurve );
        } else if( approach == "EvaluateCatmullRomSpline" ) {
            result = Curve::EvaluateCatmullRomSpline( ControlPoints, SamplesPerCurve, alpha );
        } else if( approach == "EvaluateCubicBSpline" ) {
            result = Curve::EvaluateCubicBSpline( ControlPoints, SamplesPerCurve );
        } else {
*/

TEST_CASE( "EvaluateSplines" ) {
    
    for( const auto& [ name, f, g ] : functions ) {
        SUBCASE( name.c_str() ) {
            const Points result = f( ControlPoints, SamplesPerCurve );
            
#ifdef GENERATE_DATA
            PrintToInclude( "Get" + name, "Get" + name + ".h", result );
            const Points truth = result;
#else
            const Points truth = g();
#endif
                    
            // applyTransformation() shouldn't touch face data
            CHECK( result.size() == truth.size() );
            
            // Let's find the maximum deviation of any position and use that as the score.
            real score = 0.0;
            for( int i = 0; i < std::min( result.size(), truth.size() ); ++i ) {
                const real l = ( result.at(i) - truth.at(i) ).norm();
                score = std::max( score, l );
                CHECK( l < eps );
            }
            // The ground truth data fits in an approximately 300x300 bounding box, so let's
            // divide by 300*sqrt(2) ~= 425 to get a number in the range [0,1].
            const real score_out_of_100 = (100.0*(1.0-score/425));
            // std::cerr << "Score: " << score_out_of_100 << '\n';
            MESSAGE( "Score: ", score_out_of_100 );
        }
    }
    
}
