enable_testing()
add_subdirectory(googletest)
add_executable(test main.cpp)
target_link_libraries(test lde gtest_main)
