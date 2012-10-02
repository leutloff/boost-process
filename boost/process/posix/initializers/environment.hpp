
// 
// Copyright (c) 2011 Jeff Flinn
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
        template<class Executor> void  pre_fork_parent(Executor& e) const
        {
            //TODO e.m_env_vars_ptrs.push_back(&m_namevalue);
        }

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

        environment_type m_environment;

        environment() : m_environment() {}
        template<typename T>               environment(const T& namevalue) : m_environment() { add(env(namevalue)); }
        template<typename T1, typename T2> environment(const T1& name, const T2& value) : m_environment() { add(env(name, value)); }

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

        bool operator==(const environment& rhs) const
        {
            return m_environment == rhs.m_environment;
        }


		template<class Executor> void pre_create(Executor& e) const
		{
            // TODO
#			if defined(__APPLE__)
			
				e.m_env_vars_ptrs = *_NSGetEnviron();
			
#			else
						
				e.m_env_vars_ptrs = environ;
			
#			endif
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
