# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt

if(EXISTS "${PROJECT_SOURCE_DIR}/../boost_1_48_0")
    set(BOOST_ROOT "${PROJECT_SOURCE_DIR}/../boost_1_48_0")
    set(BOOST_INCLUDEDIR "${PROJECT_SOURCE_DIR}/../boost_1_48_0")
    set(BOOST_LIBRARYDIR "${PROJECT_SOURCE_DIR}/../boost_1_48_0/stage/lib")
    message("setting BOOST_ROOT to: ${BOOST_ROOT}")
endif()

find_package(Boost COMPONENTS filesystem thread iostreams date_time system unit_test_framework REQUIRED)

include_directories(
    .
    ${PROJECT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    )

link_directories(
    "${BOOST_LIBRARYDIR}"
    )

add_executable(basic_compile_test basic_compile_test.cpp)
target_link_libraries(basic_compile_test ${Boost_LIBRARIES})

add_executable(child_process child_process.cpp)
target_link_libraries(child_process ${Boost_LIBRARIES})

add_executable(child_show_env child_show_env.cpp)
target_link_libraries(child_show_env ${Boost_LIBRARIES})

add_executable(basic_run_test basic_run_test.cpp)
target_link_libraries(basic_run_test ${Boost_LIBRARIES})

add_executable(environment_test environment_test.cpp)
target_link_libraries(environment_test ${Boost_LIBRARIES})

# fail to compile:
#add_executable(double_path_fail double_path_fail.cpp)

#add_definitions("-g")

#include(Ryppl)
#include(RypplTestSuite)

#ryppl_find_and_use_package(BoostCore)
#ryppl_find_and_use_package(BoostTest)
#ryppl_find_and_use_package(BoostTimer)

## Search tests
#ryppl_test_suite(
#  RUN
#    basic_compile_test.cpp
#    basic_run_test.cpp
#    double_path_fail.cpp
#  COMPILE
#    child_process.cpp
#  NO_SINGLE_TARGET
#  )
