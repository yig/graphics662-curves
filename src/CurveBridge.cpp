#include "CurveManager.h"

#include <emscripten.h>
#include <emscripten/bind.h>

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
