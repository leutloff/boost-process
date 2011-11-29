// Boost.Process library

// Copyright Danny havenith 2011. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org


#include <boost/thread/thread.hpp>

int main( int argc, char *argv[])
{
    boost::this_thread::sleep(boost::posix_time::seconds(10));
}
