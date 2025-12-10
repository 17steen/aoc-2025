find_package(scn REQUIRED)
find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)
find_package(ctre REQUIRED)
find_package(flux REQUIRED)
find_package(mdspan REQUIRED)
find_package(magic_enum REQUIRED)

set(DAYS
        Day01.cpp
        Day02.cpp
        Day03.cpp
        Day04.cpp
        Day05.cpp
        Day06.cpp
        Days.hpp
        Helpers.h
        LongInt.h
)

add_library(days ${DAYS})
target_link_libraries(days PRIVATE scn::scn fmt::fmt ctre::ctre flux::flux Catch2::Catch2 std::mdspan magic_enum::magic_enum)
target_compile_options(days PRIVATE -Wall -Werror)


add_executable(days-test ${DAYS})
target_link_libraries(days-test PRIVATE flux::flux scn::scn fmt::fmt ctre::ctre std::mdspan magic_enum::magic_enum Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(days-test)
