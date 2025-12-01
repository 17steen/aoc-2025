find_package(scn REQUIRED)
find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)
find_package(ctre REQUIRED)
find_package(flux REQUIRED)

set(DAYS
        Days.hpp
        Day01.cpp
        Day02.cpp
)

add_library(days ${DAYS})
target_link_libraries(days PRIVATE scn::scn fmt::fmt ctre::ctre flux::flux Catch2::Catch2)


add_executable(days-test ${DAYS})
target_link_libraries(days-test PRIVATE flux::flux scn::scn fmt::fmt ctre::ctre Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(days-test)
