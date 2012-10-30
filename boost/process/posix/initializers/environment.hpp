
// 
// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011 Jeff Flinn
// Copyright (c) 2012 Christian Leutloff
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_INITIALIZER_ENVIRONMENT_HPP
#define BOOST_PROCESS_POSIX_INITIALIZER_ENVIRONMENT_HPP

#include <boost/process/config.hpp>
#include <boost/process/posix/initializers/initializer.hpp>

#if defined(__APPLE__)
#	include <crt_externs.h> // _NSGetEnviron()
#endif

#include <map>

namespace boost { namespace process { namespace posix {

    struct executor;

    struct env : public initializer
    {
        // multiple env initializers can be combined in one sequence.
        typedef initializer_combination::ignore combination_category;

        typedef std::pair<std::string, std::string> namevalue;
        typedef boost::filesystem::path path;

        mutable namevalue m_namevalue;

        template<typename T1, typename T2> static namevalue from(const T1& name, const T2& value)
        {
            std::string n = boost::lexical_cast<std::string>(name);
            std::string v = boost::lexical_cast<std::string>(value);
            //return namevalue(n, v);
            return std::make_pair(n, v);
        }

        template<typename T> static namevalue from(const T& namevalue)
        {
            std::string nv = boost::lexical_cast<std::string>(namevalue);
            size_t pos = nv.find_first_of ('=');
            if ((std::string::npos != pos) && (nv.size() > pos))
            {
                return std::make_pair(nv.substr(0, pos), nv.substr(pos+1));
            }
            return std::make_pair(nv, "");
        }

        template<typename T1, typename T2> env(const T1& name, const T2& value) : m_namevalue(from(name, value)) {}
        template<typename T>               env(const T& namevalue) : m_namevalue(from(namevalue)) {}
                                           env(const std::string& name
                                              ,const std::string& value) : m_namevalue(name, value) {}
                                           env(const std::string& name
                                              ,const        path& p) : m_namevalue(name, p.string()) {}
//        template<class Executor> void  pre_fork_parent(Executor& e) const
//        {
//            //TODO e.m_env_vars_ptrs.push_back(&m_namevalue);
//        }

        bool operator==(const env& rhs) const
        {
            return (m_namevalue.first == rhs.m_namevalue.first) && (m_namevalue.second == rhs.m_namevalue.second);
        }

        friend std::ostream& operator<< (std::ostream& os, const env& a);
    };

    inline std::ostream& operator<< (std::ostream& os, const env& a)
    {
        os << a.m_namevalue.first << '=' << a.m_namevalue.second;
        return os;
    }

    /// start with a clean environment and avoid the initialization with the environment variables of the parent process.
    struct clean_environment {};

    /// initializate the environment variables with the values of the parent process.
    struct derive_environment {};

    struct environment : public initializer 
    {
        /**
         * Representation of a process' environment variables.
         *
         * The environment is a map that establishes an unidirectional
         * association between variable names and their values and is
         * represented by a string to string map.
         *
         * Variables may be defined to the empty string. Be aware that doing so
         * is not portable: POSIX systems will treat such variables as being
         * defined to the empty value, but Windows systems are not able to
         * distinguish them from undefined variables.
         *
         * Neither POSIX nor Windows systems support a variable with no name.
         *
         * It is worthy to note that the environment is sorted alphabetically.
         * This is provided for-free by the map container used to implement this
         * type, and this behavior is required by Windows systems.
         */
        typedef std::map<std::string, std::string> environment_type;
        typedef boost::filesystem::path path;

        // multiple environment initializers should not be combined in one sequence.
        typedef initializer_combination::exclusive combination_category;

        environment_type m_environment;

        environment()                   : m_environment() { initialize_environment(); }
        environment(derive_environment) : m_environment() { initialize_environment(); }
        environment(clean_environment)  : m_environment() { }
        template<typename T>               environment(const T& namevalue)              : m_environment() { add(env(namevalue)); }
        template<typename T1, typename T2> environment(const T1& name, const T2& value) : m_environment() { add(env(name, value)); }

        environment& operator()(derive_environment)
        {
            initialize_environment();
            return *this;
        }
        environment& operator()(clean_environment)
        {
            m_environment.clear();
            return *this;
        }
        environment& operator()(const env& e)
        {
            add(e);
            return *this;
        }
        environment& operator()(const std::string& name, const std::string& value)
        {
            add(env(name, value));
            return *this;
        }

        void add(const env& e)
        {
            environment_type::iterator lb = m_environment.lower_bound(e.m_namevalue.first);
            if ((lb != m_environment.end()) && !(m_environment.key_comp()(e.m_namevalue.first, lb->first)))
            {
                // setting a new value overwrites the existing one
                m_environment[e.m_namevalue.first] = e.m_namevalue.second;
            }
            else
            {
                m_environment.insert(lb, e.m_namevalue);
            }
        }

        void initialize_environment()
        {
#if defined(__APPLE__)
            char **ppenv = *_NSGetEnviron();
#else
            char **ppenv = environ;
#endif
            while (*ppenv)
            {
                std::string namevalue = *ppenv;
                add(env(namevalue));
                ++ppenv;
            }
        }

        void append_var(const std::string& varname, const char* value)
        {
            m_environment[varname] += value;
        }
        void append_var(const std::string& varname, const std::string& value)
        {
            m_environment[varname] += value;
        }
        void append_var(const std::string& varname, const path& p)
        {
            if (0 == m_environment.count(varname))
            {
                m_environment[varname] = p.native();
            }
            else
            {
                m_environment[varname] += ':' + p.native();
            }
        }

        void prepend_var(const std::string& varname, const char* value)
        {
            m_environment[varname] = value + m_environment[varname];
        }
        void prepend_var(const std::string& varname, const std::string& value)
        {
            m_environment[varname] = value + m_environment[varname];
        }
        void prepend_var(const std::string& varname, const path& p)
        {
            if (0 == m_environment.count(varname))
            {
                m_environment[varname] = p.native();
            }
            else
            {
                m_environment[varname] =  p.native() + ':' + m_environment[varname];
            }
        }

        bool operator==(const environment& rhs) const
        {
            return m_environment == rhs.m_environment;
        }


        /**
         * Converts an environment to a char** table as used by execve().
         *
         * Converts the environment's contents to the format used by the
         * execve() system call. The returned char** array is allocated
         * in dynamic memory; the caller must free it when not used any
         * more. Each entry is also allocated in dynamic memory and is a
         * NULL-terminated string of the form var=value; these must also be
         * released by the caller.
         *
         * \return A NULL-terminated, dynamically allocated
         *         array of dynamically allocated strings representing the
         *         enviroment's content. Each array entry is a NULL-terminated
         *         string of the form var=value. The caller is responsible for
         *         freeing them.
         */
        inline char** environment_to_envp() const
        {
            char **envp = new char*[m_environment.size() + 1];
            environment_type::size_type i = 0;
            for (environment_type::const_iterator it = m_environment.begin(); it != m_environment.end(); ++it)
            {
                std::string s = it->first + "=" + it->second;
                envp[i] = new char[s.size() + 1];
                std::strncpy(envp[i], s.c_str(), s.size() + 1);
                ++i;
            }
            envp[i] = 0;
            return envp;
        }

        template<class Executor> void pre_fork_parent(Executor& e) const
        {
            e.m_env_vars_ptrs = environment_to_envp();
        }

        friend std::ostream& operator<<(std::ostream&, const environment&);
    };

    inline std::ostream& operator<< (std::ostream& os, const environment& a)
    {
        environment::environment_type::const_iterator itBegin = a.m_environment.begin();
        environment::environment_type::const_iterator itEnd = a.m_environment.end();
        if(itBegin != itEnd)
        {
            os << itBegin->first << '=' << itBegin->second;
            ++itBegin;
        }
        for(; itBegin != itEnd; ++itBegin)
        {
            os << "\n" << itBegin->first << '=' << itBegin->second;
        }
        return os;
    }

}}}

#endif
