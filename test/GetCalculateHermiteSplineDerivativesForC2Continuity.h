
#pragma once

#include "CurveFunctions.h"

inline Curve::Points GetCalculateHermiteSplineDerivativesForC2Continuity() {
    Curve::Points result;
    result.reserve( 16 );
    result.push_back({ 403, 74 });
    result.push_back({ -189.3600137409825, 104.48849192717279 });
    result.push_back({ 242, 186 });
    result.push_back({ -104.27997251803504, 127.0230161456544 });
    result.push_back({ 217, 321 });
    result.push_back({ 48.47990381312264, 128.41944349020955 });
    result.push_back({ 323, 402 });
    result.push_back({ 153.36035726554448, 7.299209893507381 });
    result.push_back({ 459, 335 });
    result.push_back({ 64.07866712469941, -115.61628306423907 });
    result.push_back({ 430, 231 });
    result.push_back({ -88.67502576434214, -57.83407763655102 });
    result.push_back({ 334, 231 });
    result.push_back({ -84.37856406733081, 34.95259361044315 });
    result.push_back({ 270, 274 });
    result.push_back({ -53.8107179663346, 47.02370319477843 });

    return result;
}
