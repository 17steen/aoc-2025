cmake_minimum_required(VERSION 3.15)
project(aoc-2025 VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

find_package(scn REQUIRED)

add_subdirectory(days)

set(SOURCES
        src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE scn::scn days)