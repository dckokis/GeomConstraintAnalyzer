cmake_minimum_required(VERSION 3.14)
project(GeomConstraintAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(geom_analyzer_lib STATIC
        src/Variable.cpp
        src/Equation.cpp
        src/EquationSystem.cpp
        src/Analyzer.cpp
        src/Utils.cpp
)
target_include_directories(geom_analyzer_lib PUBLIC include)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE geom_analyzer_lib)

option(BUILD_TESTS "Build unit tests" ON)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()