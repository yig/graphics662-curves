cmake_minimum_required(VERSION 3.14...3.30)
project(
    Curve
    LANGUAGES CXX C
)

## Set a global C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Load packages
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    eigen
    # URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip
    # URL_HASH SHA256=1ccaabbfe870f60af3d6a519c53e09f3dcf630207321dffa553564a8e75c4fc8
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen
    # GIT_TAG 3.4.0
    ## We want a newer tag to avoid Eigen's testing polluting our testing.
    GIT_TAG 99ffad1971fb03de65f36e5401add8f872550e5f
    ## We can't use `GIT_SHALLOW TRUE` with an unnamed tag.
    GIT_SHALLOW FALSE
    GIT_PROGRESS TRUE
    )
FetchContent_MakeAvailable( eigen )

## Declare the curve library with code used by every front end
add_library(CurveLib OBJECT
    src/CurveFunctions.cpp
    src/Curve.cpp
    )
# set_target_properties( CurveLib PROPERTIES CXX_STANDARD 20 )
target_include_directories( CurveLib PUBLIC src )
target_link_libraries( CurveLib
    PUBLIC
        Eigen3::Eigen
)

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    ## Declare the Web Assembly wrapper
    add_executable( curvelib src/CurveBridge.cpp )
    target_link_libraries( curvelib
        PRIVATE
            CurveLib
            embind
    )
    
    ## Copy the executable to the `serve` directory
    set_target_properties( curvelib PROPERTIES LINK_FLAGS "-s SINGLE_FILE=1" )
    add_custom_command( TARGET curvelib POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:curvelib> ${CMAKE_SOURCE_DIR}/serve/
    )
    ## Without the `-s SINGLE_FILE=1` flag above, we would generate both a `.js` and `.wasm` file. We could copy both with:
#     add_custom_command( TARGET curvelib POST_BUILD
#       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:curvelib>/$<TARGET_FILE_BASE_NAME:curvelib>.js $<TARGET_FILE_DIR:curvelib>/$<TARGET_FILE_BASE_NAME:curvelib>.wasm ${CMAKE_SOURCE_DIR}/serve/
#     )
else()
    
    # FetchContent_Declare(
    #     spdlog
    #     GIT_REPOSITORY https://github.com/gabime/spdlog/
    #     GIT_TAG v1.x
    #     GIT_SHALLOW TRUE
    #     GIT_PROGRESS TRUE
    #     )
    # FetchContent_MakeAvailable( spdlog )
    
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
    FetchContent_MakeAvailable( json )
    
    option( COMPILE_HTTP_SERVER "Compile HTTP Server" OFF )
    if( COMPILE_HTTP_SERVER )
        FetchContent_Declare(
          httplib
          GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
          GIT_TAG v0.16.2
          GIT_SHALLOW TRUE
          GIT_PROGRESS TRUE
        )
        FetchContent_MakeAvailable( httplib )
        
        ## Declare the http server executable
        add_executable( CurveHTTPServer src/CurveHTTPServer.cpp )
        target_link_libraries( CurveHTTPServer
            PRIVATE
                CurveLib
                nlohmann_json::nlohmann_json
                httplib
                # spdlog::spdlog
        )
    
        ## Run the executable from the `serve` directory
        add_custom_target( run_HTTP
            COMMAND CurveHTTPServer
            USES_TERMINAL
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/serve
            COMMENT "Open your web browser to: http://localhost:8000/CurveHTTP.html"
            )
    endif()
    
    ## Declare the JSON server executable
    add_executable( CurveJSONServer src/CurveJSONServer.cpp )
    target_link_libraries( CurveJSONServer
        PRIVATE
            CurveLib
            nlohmann_json::nlohmann_json
            # spdlog::spdlog
    )
    
    ## Add a target to run websocketd
    add_custom_target( run_GUI
        COMMAND
            # The path to websocketd.
            # If you installed websocketd system-wide, uncomment the following line and comment line 127:
            # websocketd
            # If you placed websocketd next to `CMakeLists.txt`, uncomment the following line and comment line 125:
            ${CMAKE_SOURCE_DIR}/websocketd
            --staticdir=. --port=8000 $<TARGET_FILE:CurveJSONServer>
        DEPENDS CurveJSONServer
        USES_TERMINAL
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/serve
        COMMENT "Open your web browser to: http://localhost:8000/Curve.html"
    )
    
    option( BUILTIN_WEBSOCKET_SERVER "Built-in WebSocket Server" OFF )
    if( BUILTIN_WEBSOCKET_SERVER )
        FetchContent_Declare(
            LibDataChannel
            GIT_REPOSITORY https://github.com/paullouisageneau/libdatachannel
            GIT_TAG v0.21.2
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            )
        set(NO_EXAMPLES ON CACHE BOOL "" FORCE)
        set(NO_TESTS ON CACHE BOOL "" FORCE)
        set(NO_MEDIA ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable( LibDataChannel )
        
        ## Declare the web socket server executable
        add_executable( CurveWSServer src/CurveWSServer.cpp )
        target_link_libraries( CurveWSServer
            PRIVATE
                CurveLib
                nlohmann_json::nlohmann_json
                LibDataChannel::LibDataChannel
        )
        set_target_properties( CurveWSServer PROPERTIES CXX_STANDARD 20 )
        
        ## Run the executable from the `serve` directory
        # include(FindPython3)
        add_custom_target( run_WS
            # COMMAND ${Python3_EXECUTABLE} -m http.server -d ${CMAKE_SOURCE_DIR}/serve 8001
            COMMAND CurveWSServer
            USES_TERMINAL
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/serve
            COMMENT "Open your web browser to: http://localhost:8001/Curve.html"
            )
    endif()
    
    ## Add tests
    enable_testing()
    add_subdirectory( test )

endif()
