// Boost.Process library
// Tests related to environment variables.
//
// Copyright Christian Leutloff 2012.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE ProcessLib
#include <iostream>

// Linux requires dynamic linkage of Boost.Test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/process/process.hpp>
#include <boost/filesystem.hpp>

namespace bio = boost::iostreams;
namespace bp = boost::process;
namespace fs = boost::filesystem;

// TODO move to own file
BOOST_AUTO_TEST_CASE(prepare_arguments)
{
    {
        bp::args t(bp::arg("arg1"));
        BOOST_CHECK_EQUAL(t, "arg1");
    }
    {
        bp::args t(bp::arg(std::string("arg1 6")));
        BOOST_CHECK_EQUAL(t, "arg1 6");
    }
    {
        bp::args t = bp::arg("arg1");
        BOOST_CHECK_EQUAL(t, "arg1");
    }

    // the following tests are failing, because std::copy is used, TODO use boost::join instead
    {
        bp::args t = bp::args("arg1")("arg2");
        BOOST_CHECK_EQUAL(t, "arg1 arg2");
    }

    {
        bp::args t = bp::args("arg1")("arg2 ")("arg3");
        BOOST_CHECK_EQUAL(t, "arg1 arg2  arg3");
    }

    {
        bp::args t = bp::args("arg1")("arg2")("arg3")("arg4")("arg5")("arg6")("arg7")("arg8");
        BOOST_CHECK_EQUAL(t, "arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8");
    }
}

BOOST_AUTO_TEST_CASE(prepare_environment_variables)
{
}


BOOST_AUTO_TEST_CASE(set_environment_variables)
{
    //    // check that application to launch exists
    //    fs::path child_process =  "./child_process";
    //    BOOST_CHECK_EQUAL(true, fs::exists(child_process));

    //    using namespace boost::process;
    //    monitor m ( make_child( paths(child_process)));
    //    try
    //    {
    //        m.join();
    //    }
    //    catch (...)
    //    {
    //        std::cout << "something went wrong\n";
    //        BOOST_CHECK_MESSAGE( false, "an exception was thrown");
    //    }
}

