FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG v2.4.11
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )
FetchContent_MakeAvailable( doctest )

add_executable( evaluate evaluate.cpp )
target_link_libraries( evaluate PRIVATE CurveLib doctest::doctest )
add_test( NAME evaluate COMMAND evaluate )

## On Windows, the following line may help build a static binary that can be called from the autograder.
## Some platforms can't build entirely static binaries, so I'm leaving it off by default.
if(WIN32)
    target_link_libraries(evaluate -static)
endif()
