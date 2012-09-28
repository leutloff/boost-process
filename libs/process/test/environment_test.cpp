// Boost.Process library
// Tests related to environment variables.
//
// Copyright Christian Leutloff 2012.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE ProcessLib

// Linux requires dynamic linkage of Boost.Test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/process/process.hpp>
#include <boost/process/file_descriptor_ray.hpp>
#include <boost/filesystem.hpp>
#include <boost/tuple/tuple.hpp>

#include <iostream>
#include <sstream>

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

    {
        bp::args t = bp::args("arg1")("arg2");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "arg1 arg2");
    }

    {
        bp::args t = bp::args("arg1")("arg2 ")("arg3");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "arg1 arg2  arg3");
    }

    {
        bp::args t = bp::args("arg1")("arg2")("arg3")("arg4")("arg5")("arg6")("arg7")("arg8");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8");
    }
}

BOOST_AUTO_TEST_CASE(prepare_environment_variables)
{
}

BOOST_AUTO_TEST_CASE(set_environment_variables)
{   
    // check that application to launch exists - if this test fails, check the working directory,
    // e.g. Qt Creator: Click on Projects, select tab Run Settings and change the Working Directory to the dir with the executables.
    fs::path child_process = "./child_show_env";
    //std::cout << "child_process: " << child_process << std::endl;
    BOOST_CHECK_EQUAL(true, fs::exists(child_process));

    try
    {
        typedef bio::stream<bio::file_descriptor_source> source;
        bp::file_descriptor_ray ray;
        bp::monitor m(bp::make_child(bp::paths(child_process), bp::std_out_to(ray)));
        ray.m_sink.close(); // currently req's manual closing
        //source redirected(ray.m_source);
        //boost::tuples::tuple version(bio::parse(redirected)); // parse the istream
        // std::cout << &redirected << std::endl;
        bio::stream_buffer<bio::file_descriptor_source> pstream(ray.m_source);
        std::cout << &pstream << std::endl;

        m.join();
    }
    catch (...)
    {
        std::cout << "something went wrong\n";
        BOOST_CHECK_MESSAGE( false, "an exception was thrown");
    }
}

