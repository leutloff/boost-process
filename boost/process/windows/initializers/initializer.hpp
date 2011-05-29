
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZER_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZER_HPP
#include <boost/process/initializers/initializer_combination_category.hpp>

namespace boost { namespace process { namespace windows {

struct initializer 
{
    /// This tells us not to check for conflicts with other initializers in an
    /// InitializerSequence.
    typedef initializer_combination::ignore combination_category;

    template<class Executor> void    pre_create(Executor&) const {}
    template<class Executor> void   post_create(Executor&) const {}
    template<class Executor> void failed_create(Executor&) const {}

    struct lazy
    {
        template<class Executor> struct pre_create
        {
            Executor& m_e;

            pre_create(Executor& e) : m_e(e) {}

            template<class I> void operator()(const I& i) const { i.pre_create(m_e); }
        };
        template<class Executor> struct post_create
        {
            Executor& m_e;

            post_create(Executor& e) : m_e(e) {}

            template<class I> void operator()(const I& i) const { i.post_create(m_e); }
        };
        template<class Executor> struct failed_create
        {
            Executor& m_e;

            failed_create(Executor& e) : m_e(e) {}

            template<class I> void operator()(const I& i) const { i.failed_create(m_e); }
        };
    };
};

}}}

#endif
