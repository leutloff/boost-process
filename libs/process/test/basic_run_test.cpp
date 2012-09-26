// Boost.Process library

// Copyright Danny havenith 2011. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_TEST_MODULE ProcessLib
#include <iostream>

// Linux requires dynamic linkage of Boost Test
#define BOOST_TEST_DYN_LINK
// include Boost.Test
#include <boost/test/unit_test.hpp>

#include <boost/process/process.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

/// This does not test anything yet.
BOOST_AUTO_TEST_CASE( basic_run )
{
    std::string CHILD_NAME = "child_process";
    // check that application to launch exists
#ifdef BOOST_WINDOWS_API
    // get the name of this unit test DLL and use this path to reference the child exe
    // Reason: The unit test is executed in libs\process\test, but the DLL and the child_process are located in libs\process\test\Debug.
    char path[2048];
    GetModuleFileNameA(NULL, path, 2048); 
    fs::path dll_path(path);
    fs::path child_process = dll_path.parent_path() / CHILD_NAME;
    child_process.replace_extension(".exe");
#else
    fs::path child_process =  "./" + CHILD_NAME;
#endif
    BOOST_CHECK_EQUAL(true, fs::exists(child_process));

    using namespace boost::process;
    monitor m ( make_child( paths(child_process)));
    try
    {
        m.join();
    }
    catch (...)
    {
        std::cout << "something went wrong\n";
        BOOST_CHECK_MESSAGE( false, "an exception was thrown");
    }
}
