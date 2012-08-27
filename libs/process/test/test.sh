# test for Boost 1.50.0

g++ -c basic_compile_test.cpp -lboost_system -lboost_filesystem
g++ child_process.cpp -o child_process -lboost_system -lboost_thread-mt
g++ basic_run_test.cpp -o basic_run_test -lboost_system -lboost_filesystem -lboost_test_exec_monitor-mt
