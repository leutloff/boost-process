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
    // check that application to launch exists
    fs::path child_process =  "./child_process";
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
