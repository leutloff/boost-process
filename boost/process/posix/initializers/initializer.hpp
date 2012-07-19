
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_POSIX_INITIALIZER_HPP
#define BOOST_PROCESS_POSIX_INITIALIZER_HPP

#include <boost/process/config.hpp>
#include <boost/process/initializers/initializer_combination_category.hpp>

namespace boost { namespace process { namespace posix {

struct executor;

struct initializer 
{
    /// This tells us not to check for conflicts with other initializers in an
    /// InitializerSequence.
    template<class Executor> void    pre_fork_parent(Executor&) const {}
    template<class Executor> void   post_fork_parent(Executor&) const {}
    template<class Executor> void failed_fork_parent(Executor&) const {}
    template<class Executor> void   post_fork_child (Executor&) const {}
    template<class Executor> void failed_exec_child (Executor&) const {}

    struct lazy
    {
        template<class Executor> struct pre_fork_parent
        {
            Executor& m_e;
        
            pre_fork_parent(Executor& e) : m_e(e) {}
            
            template<class I> void operator()(const I& i) const { return i.pre_fork_parent(m_e); }
        };
        template<class Executor> struct post_fork_parent
        {
            Executor& m_e;
        
            post_fork_parent(Executor& e) : m_e(e) {}
            
            template<class I> void operator()(const I& i) const { i.post_fork_parent(m_e); }
        };
        template<class Executor> struct failed_fork_parent
        {
            Executor& m_e;
        
            failed_fork_parent(Executor& e) : m_e(e) {}
            
            template<class I> void operator()(const I& i) const { i.failed_fork_parent(m_e); }
        };
        template<class Executor> struct post_fork_child
        {
            Executor& m_e;
        
            post_fork_child(Executor& e) : m_e(e) {}
            
            template<class I> void operator()(const I& i) const { i.post_fork_child(m_e); }
        };
        template<class Executor> struct failed_exec_child
        {
            Executor& m_e;
        
            failed_exec_child(Executor& e) : m_e(e) {}
            
            template<class I> void operator()(const I& i) const { i.failed_exec_child(m_e); }
        };
    };
};

}}}

#endif
