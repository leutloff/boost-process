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
#if defined(__APPLE__)
    char **env = *_NSGetEnviron();
#else
    char **env = environ;
#endif

    while (*env)
    {
        std::string s = *env;
        std::cout <<  s << std::endl;
        ++env;
    }
    return EXIT_SUCCESS;
}

#elif defined(BOOST_WINDOWS_API)

int main(int argc, char *argv[])
{
#   ifdef GetEnvironmentStrings
#   undef GetEnvironmentStrings
#   endif

    char *ms_environ = GetEnvironmentStrings();
    if (!ms_environ)
        BOOST_PROCESS_THROW_LAST_SYSTEM_ERROR(
                    "GetEnvironmentStrings() failed");
    try
    {
        char *env = ms_environ;
        while (*env)
        {
            std::string s = env;
            std::cout <<  s << std::endl;
            env += s.size() + 1;
        }
    }
    catch (...)
    {
        FreeEnvironmentStringsA(ms_environ);
        throw;
    }
    FreeEnvironmentStringsA(ms_environ);
}

#endif
