
// 
// Copyright (c) 2011 Jeff Flinn
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_MAKE_CHILD_HPP
#define BOOST_PROCESS_MAKE_CHILD_HPP

#include <boost/process/config.hpp>

#include <boost/fusion/container/vector/vector.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/process/child.hpp>
#include <boost/process/executor.hpp>
#include <boost/process/initializers/check_initializers.hpp>

namespace boost { namespace process { 

    namespace detail
    {
        template< typename InitializerSequence>
        inline child execute( const InitializerSequence &initializers)
        {
            // Generate a compiler error if there's an inconsistency in the initializers, for instance
            // if the sequence contains two paths initializers.
            typedef typename check_initializers<InitializerSequence>::type initializers_valid;

            return executor().exec(initializers);
        }
    }

    template<typename A1>
    inline child make_child(const A1& a1)
    {
        using namespace boost::fusion;
        using boost::cref;

        return detail::execute(make_vector(cref(a1)));
    }

    template<typename A1, typename A2> 
    inline child make_child(const A1& a1, const A2& a2)
    {
        using namespace boost::fusion;
        using boost::cref;

        return detail::execute(make_vector(cref(a1), cref(a2)));
    }

    template<typename A1, typename A2, typename A3> 
    inline child make_child(const A1& a1, const A2& a2, const A3& a3)
    {
        using namespace boost::fusion;
        using boost::cref;
    
        return detail::execute(make_vector(cref(a1), cref(a2), cref(a3)));
    }

    template<typename A1, typename A2, typename A3, typename A4> 
    inline child make_child(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
    {
        using namespace boost::fusion;
        using boost::cref;

        return detail::execute(make_vector(cref(a1), cref(a2), cref(a3), cref(a4)));
    }

    template<typename A1, typename A2, typename A3, typename A4, typename A5> 
    inline child make_child(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
    {
        using namespace boost::fusion;
        using boost::cref;

        return detail::execute(make_vector(cref(a1), cref(a2), cref(a3), cref(a4), cref(a5)));
    }

    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> 
    inline child make_child(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
    {
        using namespace boost::fusion;
        using boost::cref;

        return detail::execute(make_vector(cref(a1), cref(a2), cref(a3), cref(a4), cref(a5), cref(a6)));
    }

    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7> 
    inline child make_child(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
    {
        using namespace boost::fusion;
        using boost::cref;

        return detail::execute(make_vector(cref(a1), cref(a2), cref(a3), cref(a4), cref(a5), cref(a6), cref(a7)));
    }

    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    inline child make_child(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
    {
        using namespace boost::fusion;
        using boost::cref;

        return detail::execute(make_vector(cref(a1), cref(a2), cref(a3), cref(a4), cref(a5), cref(a6), cref(a7), cref(a8)));
    }

    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    inline child make_child(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
    {
        using namespace boost::fusion;
        using boost::cref;

        return detail::execute(make_vector(cref(a1), cref(a2), cref(a3), cref(a4), cref(a5), cref(a6), cref(a7), cref(a8), cref(a9)));
    }

    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
    inline child make_child(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
    {
        using namespace boost::fusion;
        using boost::cref;

        return detail::execute(make_vector(cref(a1), cref(a2), cref(a3), cref(a4), cref(a5), cref(a6), cref(a7), cref(a8), cref(a9), cref(a10)));
    }

}}

#endif // BOOST_PROCESS_MAKE_CHILD_HPP
