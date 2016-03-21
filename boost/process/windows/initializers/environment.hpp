
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

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZER_ENVIRONMENT_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZER_ENVIRONMENT_HPP

#include <boost/process/config.hpp>
#include <boost/process/windows/initializers/initializer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_array.hpp>

#include <map>

namespace boost { namespace process { namespace windows {

    struct executor;

    struct env
    {
        typedef std::wstring name_type;
        typedef std::wstring value_type;
        typedef std::pair<name_type, value_type> namevalue_type;
        typedef boost::filesystem::path path;

        mutable namevalue_type m_namevalue;

        static namevalue_type from(const std::string& name, const std::string& value)
        {
            name_type n(name.begin(), name.end());
            value_type v(value.begin(), value.end());
            return std::make_pair(n, v);
        }

        static namevalue_type from(const std::string& namevalue)
        {
            value_type nv(namevalue.begin(), namevalue.end());
            return from(nv);
        }

        template<typename T1, typename T2> static namevalue_type from(const T1& name, const T2& value)
        {
            name_type n = boost::lexical_cast<name_type>(name);
            value_type v = boost::lexical_cast<value_type>(value);
            return std::make_pair(n, v);
        }

        template<typename T> static namevalue_type from(const T& namevalue)
        {
            value_type nv = boost::lexical_cast<value_type>(namevalue);
            size_t pos = nv.find_first_of ('=');
            if ((value_type::npos != pos) && (nv.size() > pos))
            {
                return std::make_pair(nv.substr(0, pos), nv.substr(pos+1));
            }
            return std::make_pair(nv, L"");
        }

        template<typename T1, typename T2> env(const T1& name, const T2& value) : m_namevalue(from(name, value)) {}
        template<typename T>               env(const T& namevalue) : m_namevalue(from(namevalue)) {}
                                           env(const std::wstring& name
                                              ,const std::wstring& value) : m_namevalue(name, value) {}
                                           env(const std::string& name
                                              ,const std::string& value) : m_namevalue(from(name, value)) {}
                                           env(const std::string& name
                                              ,const        path& p) : m_namevalue(from(name, p.string())) {}

        bool operator==(const env& rhs) const
        {
            return (m_namevalue.first == rhs.m_namevalue.first) && (m_namevalue.second == rhs.m_namevalue.second);
        }

        friend std::wostream& operator<< (std::wostream& os, const env& a);
        friend std::ostream& operator<< (std::ostream& os, const env& a);
    };

    inline std::wostream& operator<< (std::wostream& os, const env& a)
    {
        os << a.m_namevalue.first << '=' << a.m_namevalue.second;
        return os;
    }

    inline std::ostream& operator<< (std::ostream& os, const env& a)
    {
        std::string name(a.m_namevalue.first.begin(), a.m_namevalue.first.end());
        std::string value(a.m_namevalue.second.begin(), a.m_namevalue.second.end());
        os << name << '=' << value;
        return os;
    }

    /// start with a clean environment and avoid the initialization with the environment variables of the parent process.
    struct clean_environment {};

    /// initializate the environment variables with the values of the parent process.
    struct derive_environment {};

    struct environment : public initializer 
    {
        struct case_insensitve_sort
        {
            bool operator()(std::wstring a, std::wstring b) const
            {
                return _wcsicmp(a.c_str(), b.c_str()) < 0;
            }
        };


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
        typedef std::map<std::wstring, std::wstring, case_insensitve_sort> environment_type;
        // multiple environment initializers should not be combined in one sequence.
        typedef initializer_combination::exclusive combination_category;
        typedef boost::filesystem::path path;

        environment_type m_environment;

        environment()                   : m_environment() { initialize_environment(); }
        environment(derive_environment) : m_environment() { initialize_environment(); }
        environment(clean_environment)  : m_environment() { }
        template<typename T>               environment(const T& namevalue)              : m_environment() { add(env(namevalue)); }
        template<typename T1, typename T2> environment(const T1& name, const T2& value) : m_environment() { add(env(name, value)); }

        ~environment()
        { }

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
        environment& operator()(const std::wstring& name, const std::wstring& value)
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
            wchar_t *environ = GetEnvironmentStringsW();
            if (!environ)
            {
                boost::throw_exception(boost::system::system_error(boost::system::error_code(GetLastError(), boost::system::system_category()),
                           "initialize_environment() failed"));
            }
            try
            {
                wchar_t *penv = environ;
                while (*penv)
                {
                    std::wstring namevalue = penv;
                    add(env(namevalue));
                    penv += namevalue.size() + 1;
                }
            }
            catch (...)
            {
                FreeEnvironmentStringsW(environ);
                throw;
            }
            FreeEnvironmentStringsW(environ);
        }

        bool operator==(const environment& rhs) const
        {
            return m_environment == rhs.m_environment;
        }


        template<class Executor> void pre_create(Executor& e) const
        {
            e.m_creation_flags |= CREATE_UNICODE_ENVIRONMENT;
            e.m_env_vars = environment_to_windows_strings();
        }

        /**
         * Converts an environment to a string used by CreateProcessW().
         *
         * Converts the environment's contents to the format used by the
         * CreateProcessW() system call. The returned wchar_t* string is
         * allocated in dynamic memory; the caller must free it when not
         * used any more. This is enforced by the use of a shared pointer.
         *
         * This operation is only available on Windows systems.
         *
         * \return A dynamically allocated wchar_t* string that represents
         *         the environment's content. This string is of the form
         *         var1=value1\\0var2=value2\\0\\0.
         */
        inline boost::shared_array<wchar_t> environment_to_windows_strings() const
        {
            boost::shared_array<wchar_t> envp;

            if (m_environment.empty())
            {
                const size_t size = 2;
                envp.reset(new wchar_t[size]);
                memset(envp.get(), 0, size*sizeof(wchar_t));
            }
            else
            {
                size_t size = 2;    // add two closing \0
                for (environment_type::const_iterator it = m_environment.begin(); 
                     m_environment.end() != it;
                     ++it)
                {
                    //   += it->first + L'=' + it->second + L'\0'
                    size += it->first.length() + it->second.length() + 2;
                }

                // smaller buffer are not working with memcpy_s
                if (54 > size) { size = 54; } 

                envp.reset(new wchar_t[size]);
                memset(envp.get(), 0, size*sizeof(wchar_t));
                std::wstring ws;
                wchar_t* b = envp.get();
                for (environment_type::const_iterator it = m_environment.begin(); 
                     m_environment.end() != it;
                     ++it)
                {
                    ws = it->first + L"=" + it->second;
                    const size_t n = ws.length() * sizeof(wchar_t);
#if (BOOST_MSVC >= 1400) && !defined(ABC)
                    memcpy_s(b, n, ws.c_str(), n);
#else
                    memcpy(b, ws.c_str(), n);
#endif
                    b += ws.length() + 1;
                }
            }

            return envp;
        }

        void append_var(const std::string& varname, const char* value)
        {
            std::string v(value);
            append_var(varname, v);
        }
        void append_var(const std::string& varname, const std::string& value)
        {
            std::wstring wn(varname.begin(), varname.end());
            std::wstring wv(value.begin(), value.end());
            append_var(wn, wv);
        }
        void append_var(const std::wstring& varname, const wchar_t* value)
        {
            m_environment[varname] += value;
        }
        void append_var(const std::wstring& varname, const std::wstring& value)
        {
            m_environment[varname] += value;
        }
        void append_var(const std::string& varname, const path& p)
        {
            std::wstring wn(varname.begin(), varname.end());
            if (0 == m_environment.count(wn))
            {
                m_environment[wn] = p.native();
            }
            else
            {
                m_environment[wn] += L';' + p.native();
            }
        }

        void prepend_var(const std::string& varname, const char* value)
        {
            std::string v(value);
            prepend_var(varname, v);
        }
        void prepend_var(const std::string& varname, const std::string& value)
        {
            std::wstring wn(varname.begin(), varname.end());
            std::wstring wv(value.begin(), value.end());
            prepend_var(wn, wv);
        }
        void prepend_var(const std::wstring& varname, const wchar_t* value)
        {
            m_environment[varname] = value + m_environment[varname];
        }
        void prepend_var(const std::wstring& varname, const std::wstring& value)
        {
            m_environment[varname] = value + m_environment[varname];
        }
        void prepend_var(const std::string& varname, const path& p)
        {
            std::wstring wn(varname.begin(), varname.end());
            if (0 == m_environment.count(wn))
            {
                m_environment[wn] = p.native();
            }
            else
            {
                m_environment[wn] =  p.native() + L';' + m_environment[wn];
            }
        }

        friend std::wostream& operator<<(std::wostream&, const environment&);
        friend std::ostream& operator<<(std::ostream&, const environment&);

    };

    inline std::wostream& operator<< (std::wostream& os, const environment& a)
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

    inline std::ostream& operator<< (std::ostream& os, const environment& a)
    {
        environment::environment_type::const_iterator itBegin = a.m_environment.begin();
        environment::environment_type::const_iterator itEnd = a.m_environment.end();
        if(itBegin != itEnd)
        {
            std::string name(itBegin->first.begin(), itBegin->first.end());
            std::string value(itBegin->second.begin(), itBegin->second.end());
            os << name << '=' << value;
            ++itBegin;
        }
        for(; itBegin != itEnd; ++itBegin)
        {
            std::string name(itBegin->first.begin(), itBegin->first.end());
            std::string value(itBegin->second.begin(), itBegin->second.end());
            os << "\n" << name << '=' << value;
        }
        return os;
    }


}}}

#endif
