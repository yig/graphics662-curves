#ifndef __jsassert_h__
#define __jsassert_h__

#include <string>
#include <sstream>

#include "emscripten.h"

inline void jsAlert( const std::string& msg ) {
    EM_ASM({ alert( UTF8ToString($0) ); }, msg.c_str() );
}
inline void jsLog( const std::string& msg ) {
    EM_ASM({ console.log( UTF8ToString($0) ); }, msg.c_str() );
}
inline void jsWarn( const std::string& msg ) {
    EM_ASM({ console.warn( UTF8ToString($0) ); }, msg.c_str() );
}
inline void jsError( const std::string& msg ) {
    EM_ASM({ console.error( UTF8ToString($0) ); }, msg.c_str() );
}

#ifdef NDEBUG
    #define jsAssert( cond ) do {} while(0)
#else
    #define jsAssert( cond ) \
    do { \
        if( !(cond) ) { \
            jsAlert( std::string("assert ") + std::string(#cond) + std::string(" failed at ") + std::string(__FILE__) + std::string(":") + toString(__LINE__) ); \
        } \
    } while(0)
#endif

template< typename T > std::string toString( const T& t )
{
    std::ostringstream sstr;
    sstr << t;
    return sstr.str();
}

#endif /* __jsassert_h__ */
