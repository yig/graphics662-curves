
#pragma once

#include "CurveFunctions.h"

inline Curve::Points GetComputeBSplineFromInterpolatingPoints() {
    Curve::Points result;
    result.reserve( 10 );
    result.push_back({ 592.3600137409826, -30.488491927172788 });
    result.push_back({ 403, 73.99999999999999 });
    result.push_back({ 213.63998625901746, 178.48849192717276 });
    result.push_back({ 194.44005496392995, 328.0460322913089 });
    result.push_back({ 310.5997938852628, 435.3273789075919 });
    result.push_back({ 501.1607694950188, 342.6444520783236 });
    result.push_back({ 438.7571281346616, 204.09481277911368 });
    result.push_back({ 323.81071796633455, 226.97629680522155 });
    result.push_back({ 270, 273.99999999999994 });
    result.push_back({ 216.18928203366542, 321.02370319477836 });

    return result;
}
