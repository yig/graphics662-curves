#ifndef __jsassert_h__
#define __jsassert_h__

#include <string>
#include <sstream>

#ifdef NDEBUG
    #define jsAssert( cond ) do {} while(0)
#else
    extern void jsAlert( const std::string& msg );
    
    #define jsAssert( cond ) \
    do { \
        if( !(cond) ) { \
            jsAlert( std::string("assert ") + #cond + std::string(" failed at ") + __FILE__ + std::string(":") + __LINE__ ); \
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
