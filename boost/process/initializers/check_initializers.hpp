
// 
// Copyright (c) 2011 Danny Havenith
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_CHECK_INITIALIZERS_HPP
#define BOOST_PROCESS_CHECK_INITIALIZERS_HPP
#include <boost/process/initializers/initializer_combination_category.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/sequence/intrinsic/empty.hpp>
#include <boost/fusion/sequence/intrinsic/front.hpp>
#include <boost/fusion/algorithm/transformation/pop_front.hpp>
#include <boost/static_assert.hpp>

namespace boost { namespace process { namespace detail {
    namespace fro = boost::fusion::result_of;
    namespace mpl = boost::mpl;

    ///
    /// Second order meta function that returns a meta function that checks an InitializerSequence
    /// for conflicts with one of its Initializer types.
    template<
        typename ReferenceInitializer,
        typename Category = typename initializer_combination_category< typename boost::remove_reference<ReferenceInitializer>::type >::type
        >
    struct initializer_checker
    {
        /// The default implementation does not perform any checks on the range
        template< typename >
        struct apply
        {
            typedef boost::mpl::true_ type;
        };
    };


    /// Specialization for exclusive initializers.
    /// In this case we do not allow any initializers of the same type in the InitializerSequence
    template< typename Initializer>
    struct initializer_checker<Initializer, initializer_combination::exclusive>
    {
        /// Meta function to test for occurrences of Initializer in InitializerSequence
        template< typename InitializerSequence, bool Empty = fro::empty<InitializerSequence>::type::value, typename Dummy = void>
        struct apply
        {
            typedef typename
                    mpl::not_<
                        typename mpl::contains< InitializerSequence, Initializer>::type
                        >::type type;

            // It may be a bit weird to assert here, in the middle of the checker, but this allows us to generate more specific
            // assertions that just 'something is not right with your initializer sequence'.

            // if you get a compilation error here, you have provided two initializers of the same type to make_child, where that
            // doesn't make sense.
            BOOST_MPL_ASSERT_MSG(type::value, INITIALIZER_OCCURS_MORE_THAN_ONCE, (Initializer));
        };

        template< typename InitializerSequence, typename Dummy>
        struct apply<InitializerSequence, true, Dummy>
        {
            typedef boost::mpl::true_ type;

        };
    };


    /// Meta function to check a sequence of initializers for conflicts.
    /// An example of a conflict is having two path-initializers in the sequence.
    /// This function will determine the first element of the sequence (the Head) and find an appropriate checker, based
    /// on the combination_category of that first element. Typically the combination category is either 'ignore' or 'exclusive'.
    /// The checker is then used to test the remainder (the Tail) of the sequence for conflicts with the first element.
    /// After that, this function recurses into the Tail.
    template< typename InitializerSequence, bool Empty = fro::empty<InitializerSequence>::type::value >
    struct check_initializers
    {
        typedef typename fro::front< InitializerSequence>::type      Head;
        typedef typename fro::pop_front< InitializerSequence>::type  Tail;

        /// Determine which Checker to use for the Head
        typedef initializer_checker< Head> Checker;

        /// Apply the checker to the Tail
        typedef typename Checker:: template apply< Tail>::type HeadResult;

        /// Recurse into the rest of the sequence.
        typedef typename
                boost::mpl::and_<
                    HeadResult,
                    typename check_initializers< Tail>::type
                  >::type type;

    };

    /// empty initializer sequences are all right, we're ending recursion.
    template< typename InitializerSequence>
    struct check_initializers< InitializerSequence, true>
    {
        typedef boost::mpl::true_ type;
    };

}}} // end namespace boost::process::detail
#endif // BOOST_PROCESS_CHECK_INITIALIZERS_HPP
