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

if(MSVC)
    add_definitions("-D_SCL_SECURE_NO_WARNINGS")
    add_definitions("-D_CRT_SECURE_NO_DEPRECATE")

#boost_filesystem-vc100-mt-gd-1_48.dll
    # Execute the INSTALL project to copy the required boost libs to the Debug and Release directories.

    get_filename_component(FS_BASE_NAME_REL ${Boost_FILESYSTEM_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(FS_BASE_NAME_DBG ${Boost_FILESYSTEM_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(THRD_BASE_NAME_REL ${Boost_THREAD_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(THRD_BASE_NAME_DBG ${Boost_THREAD_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(DT_BASE_NAME_REL ${Boost_DATE_TIME_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(DT_BASE_NAME_DBG ${Boost_DATE_TIME_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(SYS_BASE_NAME_REL ${Boost_SYSTEM_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(SYS_BASE_NAME_DBG ${Boost_SYSTEM_LIBRARY_DEBUG} NAME_WE)
    get_filename_component(UTF_BASE_NAME_REL ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE} NAME_WE)
    get_filename_component(UTF_BASE_NAME_DBG ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG} NAME_WE)

    install(FILES ${BOOST_LIBRARYDIR}/${FS_BASE_NAME_DBG}.dll
                  ${BOOST_LIBRARYDIR}/${THRD_BASE_NAME_DBG}.dll
                  ${BOOST_LIBRARYDIR}/${DT_BASE_NAME_DBG}.dll
                  ${BOOST_LIBRARYDIR}/${SYS_BASE_NAME_DBG}.dll
                  ${BOOST_LIBRARYDIR}/${UTF_BASE_NAME_DBG}.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug
        CONFIGURATIONS Debug
    )
    install(FILES ${BOOST_LIBRARYDIR}/${FS_BASE_NAME_REL}.dll
                  ${BOOST_LIBRARYDIR}/${THRD_BASE_NAME_REL}.dll
                  ${BOOST_LIBRARYDIR}/${DT_BASE_NAME_REL}.dll
                  ${BOOST_LIBRARYDIR}/${SYS_BASE_NAME_REL}.dll
                  ${BOOST_LIBRARYDIR}/${UTF_BASE_NAME_REL}.dll
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release
        CONFIGURATIONS Release RelWithDebInfo
    )
    
endif()
    
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

add_executable(execute_show_env execute_show_env.cpp)
target_link_libraries(execute_show_env ${Boost_LIBRARIES})

# fail to compile:
#add_executable(double_path_fail double_path_fail.cpp)

add_definitions("-g")

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
