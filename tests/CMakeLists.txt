include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)

FetchContent_MakeAvailable(googletest)

add_executable(Tests
        TestAnalyzer.cpp
)

target_link_libraries(Tests
        PRIVATE
        geom_analyzer_lib
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(Tests)