#include <boost/process/process.hpp>

void f()
{
    using namespace boost::process;

    make_child( paths("a"), paths("b") );
}
