enable_testing()
add_subdirectory(googletest)
add_executable(test EXCLUDE_FROM_ALL main.cpp)
target_link_libraries(test lde gtest_main)
