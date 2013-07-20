boost-process
=============

[![Build Status](https://travis-ci.org/leutloff/boost-process.png)](https://travis-ci.org/leutloff/boost-process)

Proposed portable process library for Boost (see [http://boost.org](http://boost.org)).

The library is header-only. There are some examples in the test cases
in libs/process/test. They can be build using cmake, e.g.

    mkdir build && cd build && cmake .. && make

The the tests can be executed:

    pushd libs/process/test && ./basic_run_test && ./environment_test && ./execute_show_env && popd

More information about the library is available from the BoostProcess.pdf file.
But some of the descriptions may be outdated.
