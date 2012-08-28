// Boost.Process library

// Copyright Danny havenith 2011. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_TEST_MODULE ProcessLib
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/process/process.hpp>

/// This does not test anything yet.
BOOST_AUTO_TEST_CASE( basic_run )
{

    using namespace boost::process;
    //monitor m ( make_child( paths("./child_process")));
    monitor m ( make_child( paths(".\\child_process.exe")));
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
