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


// TODO move to own file?
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

#if defined(BOOST_POSIX_API)
    // can use unquoted arguments - TODO: is this difference between POSIX and WINDOWS intentional?
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
#elif defined(BOOST_WINDOWS_API)
    // uses quoted arguments
    {
        bp::args t = bp::args("arg1")("arg2");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "\"arg1\" \"arg2\"");
    }

    {
        bp::args t = bp::args("arg1")("arg2 ")("arg3");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "\"arg1\" \"arg2 \" \"arg3\"");
    }

    {
        bp::args t = bp::args("arg1")("arg2")("arg3")("arg4")("arg5")("arg6")("arg7")("arg8");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "\"arg1\" \"arg2\" \"arg3\" \"arg4\" \"arg5\" \"arg6\" \"arg7\" \"arg8\"");
    }
#else
#   error "Unsupported platform."
#endif
}

BOOST_AUTO_TEST_CASE(prepare_environment_variables)
{
    {
        bp::env t("env1", "val1");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "env1=val1");
    }
    {
        bp::environment t(bp::env("env1", "val1"));
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "env1=val1");
    }
    {
        bp::environment t(bp::env(std::string("env1=6")));
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "env1=6");
    }
    {
        bp::environment t = bp::env("env1", "val1");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "env1=val1");
    }

    {
        bp::environment t = bp::environment("env1=val1")("env2=val2");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "env1=val1\nenv2=val2");
    }
    {
        bp::environment t = bp::environment("env1", "val1")("env2", "val2");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "env1=val1\nenv2=val2");
    }

    {
        bp::environment t = bp::environment("env1", "val1")("env2", "val2")("env3", "val3");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "env1=val1\nenv2=val2\nenv3=val3");
    }

    {
        bp::environment t = bp::environment("env1", "val1")("env2", "val2")("env3", "val3")("env4", "val4")("env5", "val5")("env6", "val6")("env7", "val7")("env8", "val8");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "env1=val1\nenv2=val2\nenv3=val3\nenv4=val4\nenv5=val5\nenv6=val6\nenv7=val7\nenv8=val8");
    }

    {
        bp::environment t = bp::environment("env1", "val1")("env2", "val2")("env3=val3")("env4=val4")("env5", "val5")("env6", "val6")("env7", "val7")("env8", "val8");
        std::ostringstream oss;
        oss << t;
        BOOST_CHECK_EQUAL(oss.str(), "env1=val1\nenv2=val2\nenv3=val3\nenv4=val4\nenv5=val5\nenv6=val6\nenv7=val7\nenv8=val8");
    }
}

void check_equal(bp::environment const& e, std::string const& expected)
{
    std::ostringstream oss;
    oss << e;
    std::string s = oss.str();
    BOOST_CHECK_MESSAGE(s == expected, s + " != (expected:) " + expected);
}

void check_contains(bp::environment const& e, std::string const& contains)
{
    std::ostringstream oss;
    oss << e;
    std::string s = oss.str();
    BOOST_CHECK_MESSAGE(std::string::npos != s.find(contains), s + " != (expected:) " + contains);
}

BOOST_AUTO_TEST_CASE(clean_or_derived_environment)
{
    {
        bp::environment t = bp::environment(bp::derive_environment())("env1", "val1")("env2", "val2")("env3", "val3");
        check_contains(t, "env1=val1\nenv2=val2\nenv3=val3");
        // check for common environment variables on Posix and Windows systems
        check_contains(t, "\nPATH=");
#if defined(BOOST_POSIX_API)
        check_contains(t, "\nHOME=");
#elif defined(BOOST_WINDOWS_API)
        check_contains(t, "\nUSERNAME=");
#else
#   error "Unsupported platform."
#endif
    }
    {
        bp::environment t = bp::environment(bp::derive_environment())("env1", "val1")("env2", "val2")("env3", "val3");
        check_contains(t, "env1=val1\nenv2=val2\nenv3=val3");
        check_contains(t, "\nPATH=");
#if defined(BOOST_POSIX_API)
        check_contains(t, "\nHOME=");
#elif defined(BOOST_WINDOWS_API)
        check_contains(t, "\nUSERNAME=");
#endif
    }
    {
        bp::environment t = bp::environment(bp::clean_environment())("env1", "val1")("env2", "val2")("env3", "val3");
        check_equal(t, "env1=val1\nenv2=val2\nenv3=val3");
    }
    {
        bp::environment t = bp::environment(bp::clean_environment())("env3", "val3")("env2", "val2")("env1", "val1");
        check_equal(t, "env1=val1\nenv2=val2\nenv3=val3");
    }
}

BOOST_AUTO_TEST_CASE(append_to_envvar)
{
    // add to an existing environment variable
    {
        bp::environment t = bp::environment(bp::clean_environment())("env2", "val2")("env1", "val1");
        check_equal(t, "env1=val1\nenv2=val2"); // checks the prerequisits of the following tests
        t.append_var("env1", "app1");
        check_equal(t, "env1=val1app1\nenv2=val2");
        t.append_var("env2", "app2");
        check_equal(t, "env1=val1app1\nenv2=val2app2");
    }
    {
        bp::environment t = bp::environment(bp::clean_environment());
        check_equal(t, "");
        t.append_var("env1", "app1");
        check_equal(t, "env1=app1");
        t.append_var("env2", "app2");
        check_equal(t, "env1=app1\nenv2=app2");
    }
    // "add" to an non-existing environment variable
    {
        bp::environment t = bp::environment(bp::clean_environment())("env2", "val2")("env1", "val1");
        check_equal(t, "env1=val1\nenv2=val2"); // checks the prerequisits of the following tests
        t.append_var("env1", "app1");
        check_equal(t, "env1=val1app1\nenv2=val2");
        t.append_var("env2", "app2");
        check_equal(t, "env1=val1app1\nenv2=val2app2");
    }
    {
        bp::environment t = bp::environment(bp::clean_environment());
        check_equal(t, "");
        t.append_var("env1", "app1");
        check_equal(t, "env1=app1");
        t.append_var("env2", "app2");
        check_equal(t, "env1=app1\nenv2=app2");
    }
    // add a path to an existing environment variable
    {
        bp::environment t = bp::environment(bp::clean_environment())("env2", "val2")("env1", "val1");
        check_equal(t, "env1=val1\nenv2=val2"); // checks the prerequisits of the following tests
        t.append_var("env1", fs::path("app1"));
#if defined(BOOST_POSIX_API)
        check_equal(t, "env1=val1:app1\nenv2=val2");
#elif defined(BOOST_WINDOWS_API)
        check_equal(t, "env1=val1;app1\nenv2=val2");
#endif
        t.append_var("env2", fs::path("app2"));
#if defined(BOOST_POSIX_API)
        check_equal(t, "env1=val1:app1\nenv2=val2:app2");
#elif defined(BOOST_WINDOWS_API)
        check_equal(t, "env1=val1;app1\nenv2=val2;app2");
#endif
    }
    {
        bp::environment t = bp::environment(bp::clean_environment());
        check_equal(t, "");
        t.append_var("env1", "app1");
        check_equal(t, "env1=app1");
        t.append_var("env2", "app2");
        check_equal(t, "env1=app1\nenv2=app2");
    }
}

BOOST_AUTO_TEST_CASE(prepend_to_envvar)
{
    {
        bp::environment t = bp::environment(bp::clean_environment())("env2", "val2")("env1", "val1");
        check_equal(t, "env1=val1\nenv2=val2");
        t.prepend_var("env1", "pre1");
        check_equal(t, "env1=pre1val1\nenv2=val2");
        t.prepend_var("env2", "pre2");
        check_equal(t, "env1=pre1val1\nenv2=pre2val2");
    }
    {
        bp::environment t = bp::environment(bp::clean_environment());
        check_equal(t, "");
        t.prepend_var("env1", "pre1");
        check_equal(t, "env1=pre1");
        t.prepend_var("env2", "pre2");
        check_equal(t, "env1=pre1\nenv2=pre2");
    }
    {
        bp::environment t = bp::environment(bp::clean_environment())("env2", "val2")("env1", "val1");
        check_equal(t, "env1=val1\nenv2=val2");
        t.prepend_var("env1", "pre1");
        check_equal(t, "env1=pre1val1\nenv2=val2");
        t.prepend_var("env2", "pre2");
        check_equal(t, "env1=pre1val1\nenv2=pre2val2");
    }
    {
        bp::environment t = bp::environment(bp::clean_environment());
        check_equal(t, "");
        t.prepend_var("env1", "pre1");
        check_equal(t, "env1=pre1");
        t.prepend_var("env2", "pre2");
        check_equal(t, "env1=pre1\nenv2=pre2");
    }
}

BOOST_AUTO_TEST_CASE(set_environment_variables)
{   
    // check that application to launch exists - if this test fails, check the working directory,
    // e.g. Qt Creator: Click on Projects, select tab Run Settings and change the Working Directory to the dir with the executables.
    // or Visual Studio: Open the Project Properties, select Debugging and change the Working Directory from $(ProjectDir) to $(OutDir).
#if defined(BOOST_POSIX_API)
    fs::path child_process = "./child_show_env";
#elif defined(BOOST_WINDOWS_API)
    fs::path child_process = "child_show_env.exe";
#endif
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
    catch (std::exception const& ex)
    {
        std::ostringstream oss;
        oss << "something went wrong: " << ex.what();
        std::cout << oss.str() << std::endl;
        BOOST_CHECK_MESSAGE(false,  oss.str());
    }
    catch (...)
    {
        std::cout << "something went wrong\n";
        BOOST_CHECK_MESSAGE(false, "an exception was thrown");
    }
}

