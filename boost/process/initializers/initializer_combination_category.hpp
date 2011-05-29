
// 
// Copyright (c) 2011 Danny Havenith
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_PROCESS_INITIALIZER_COMBINATION_CATEGORY_HPP
#define BOOST_PROCESS_INITIALIZER_COMBINATION_CATEGORY_HPP

namespace boost { namespace process {

    /// This namespace holds the initializer combination categories.
    namespace initializer_combination {

        /// No two initializers of the same type can be in the same
        /// InitializerSequence
        struct exclusive {};

        /// No check is performed for initializers of this category.
        struct ignore {};
    }

    /// Meta function that returns the combination category of an initializer
    template <typename Initializer>
    struct initializer_combination_category
    {
        typedef typename Initializer::combination_category type;
    };

}}
#endif // BOOST_PROCESS_INITIALIZER_COMBINATION_CATEGORY_HPP
