// Boost.Process library
// Program to call the client showing the environment variables on standard output.
// Implements direct call and boost::process usage for comparison.
//
// Copyright Christian Leutloff 2012.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/process/config.hpp>
#include <boost/process/executor.hpp>
#include <boost/process/make_child.hpp>
#include <boost/process/monitor.hpp>
#include <boost/process/file_descriptor_ray.hpp>
#include <boost/process/initializers/environment.hpp>
#include <boost/process/initializers/paths.hpp>
#include <boost/process/initializers/std_io.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/tuple/tuple.hpp>

#if defined(BOOST_POSIX_API)
#   include <unistd.h>
#   include <stdlib.h>
#   include <sys/types.h>
#   include <sys/wait.h>

#   if defined(__APPLE__)
#       include <crt_externs.h>
#   endif
//#elif defined(BOOST_WINDOWS_API)
//#   include <windows.h>
#else
#   error "Unsupported platform."
#endif
#include <iostream>


#if defined(BOOST_POSIX_API)

int main(int argc, char *argv[])
{
    using namespace std;
    typedef boost::filesystem::path            path;
    typedef char                               char_type;
    typedef boost::process::executor::arg_ptrs arg_ptrs;
    typedef boost::process::environment        environment;
    typedef boost::process::env                env;

    int status = -1;
    char exe[] = "child_show_env";
    path                       m_exe = exe;
    path                       parent_exe = argv[0];
    path                       m_working_dir = parent_exe.branch_path();
    arg_ptrs                   m_arg_ptrs;

    environment e("USER", "user1");
    e.add(env("PATH=/usr/bin:/bin:/opt/bin"));
    char_type**                m_env_vars_ptrs = e.environment_to_envp();

    //m_arg_ptrs.assign_app(m_exe.c_str()); // TODO why is assign_app not accepting a const char*?
    m_arg_ptrs.assign_app(exe);
//    m_arg_ptrs.push_back("-l");
//    m_arg_ptrs.push_back("-a");
//    m_arg_ptrs.push_back("/tmp");


    pid_t m_id = fork();
    if (m_id == 0)
    {
        std::cout << "2 - Forked process" << std::endl;
        if (chdir(m_working_dir.c_str()) == -1)
        {
            cerr << "2 - chdir failed." << std::endl;
        }
        int ret = execve (m_exe.c_str(), m_arg_ptrs.ptr(), m_env_vars_ptrs);

        // this code is only executed when execve fails
        cerr << "2 - execve failed with error code: " << ret << std::endl;
        _exit(0); // exit forked process
    }
    else if (m_id < 0) //  to fork
    {
        cerr << "1 - Fork failed!" << std::endl;
    }
    else
    {
        std::cout << "1 - Parent process (" << argv[0] << ") waits for the forked process to return" << std::endl;
        pid_t wait_val = 0;
        do
        {
            wait_val = waitpid(m_id, &status, 0);
        }
        while(wait_val == -1 && errno == EINTR);

        if(wait_val == -1)
        {
            cerr << "1 - Waiting for client failed with errno: " << errno << std::endl;
        }
        else
        {
            std::cout << "1 - Client completed with status: " << status << std::endl;
        }


        // now the same againg
        try
        {
            std::cout << "1 - Now doing the same with boost::process itself ..." << std::endl;
            namespace bp=boost::process;
            namespace bio = boost::iostreams;

            typedef bio::stream<bio::file_descriptor_source> source;
            bp::file_descriptor_ray ray;
            bp::monitor m(bp::make_child(bp::paths(exe, m_working_dir.c_str()), e, bp::std_out_to(ray)));
            ray.m_sink.close(); // currently req's manual closing
            bio::stream_buffer<bio::file_descriptor_source> pstream(ray.m_source);
            // is working, too: std::cout << &pstream << std::endl;
            std::ostringstream oss;
            oss << &pstream;
            std::cout << oss.str() << std::endl;
            m.join();
        }
        catch (std::exception const& ex)
        {
            std::cout << "something went wrong: " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cout << "something went wrong" << std::endl;
        }
    }
    return status;
}

#elif defined(BOOST_WINDOWS_API)

int main(int argc, char *argv[])
{
 // TODO
}

#endif
