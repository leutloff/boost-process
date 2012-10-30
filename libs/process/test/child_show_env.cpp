// Boost.Process library
// Program to show the environment variables on standard output.
//
// Accessing the environment is based on deprecated/process-0.4/boost/process/self.hpp
// Copyright Christian Leutloff 2012.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/process/config.hpp>
#include <boost/system/error_code.hpp> 
#include <boost/system/system_error.hpp>
#include <boost/throw_exception.hpp>

#if defined(BOOST_POSIX_API)
#   include <stdlib.h>
#   if defined(__APPLE__)
#       include <crt_externs.h>
#   endif
#elif defined(BOOST_WINDOWS_API)
#   include <windows.h>
#else
#   error "Unsupported platform."
#endif
#include <iostream>

#if defined(BOOST_POSIX_API)
extern "C"
{
extern char **environ;
}

int main(int argc, char *argv[])
{
    std::cout << argv[0] << std::endl;

    int i = 0;
#if defined(__APPLE__)
    char **env = *_NSGetEnviron();
#else
    char **env = environ;
#endif
    while (*env)
    {
        std::string s = *env;
        std::cout << s << std::endl;
        ++env;
        ++i;
    }
    std::cout << "Number of Environment Variables: " << i << std::endl;
    return EXIT_SUCCESS;
}

#elif defined(BOOST_WINDOWS_API)

int main(int argc, char *argv[])
{
    std::cout << argv[0] << std::endl;

    int i = 0;
    char *ms_environ = GetEnvironmentStringsA();
    if (!ms_environ)
    {
        boost::throw_exception(boost::system::system_error(boost::system::error_code(GetLastError(), boost::system::get_system_category()),
                   "GetEnvironmentStringsA() failed"));
    }
    try
    {
        char *env = ms_environ;
        while (*env)
        {
            std::string s = env;
            std::cout << s << std::endl;
            env += s.size() + 1;
            ++i;
        }
    }
    catch (...)
    {
        FreeEnvironmentStringsA(ms_environ);
        throw;
    }
    FreeEnvironmentStringsA(ms_environ);
    std::cout << "Number of Environment Variables: " << i << std::endl;
}

#endif
