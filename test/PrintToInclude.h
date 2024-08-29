#pragma once

#include "CurveFunctions.h"
#include <fstream>
#include <string>
#include <format>

// Prints the curve data as a C++ file that can be included.
inline void PrintToInclude( const std::string& functionName, const std::string& outpath, const Curve::Points& pts ) {
    std::ofstream out( outpath );
    // https://stackoverflow.com/questions/554063/how-do-i-print-a-double-value-with-full-precision-using-cout
    out.precision(17);
    
    out << std::format( R"(
#pragma once

#include "CurveFunctions.h"

inline Curve::Points {}() {{
    Curve::Points result;
    result.reserve( {} );
)", functionName, pts.size() );
    
    for( const auto& p : pts ) {
        out << std::format( "    result.push_back({{ {}, {} }});\n", p.x(), p.y() );
    }
    
    out << R"(
    return result;
}
)";
}
