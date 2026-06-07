cmake_minimum_required(VERSION 3.16)
project(ripple_package_test LANGUAGES CXX)

find_package(ripple CONFIG REQUIRED)

add_executable(ripple_package_test package_test.cpp)
target_link_libraries(ripple_package_test PRIVATE ripple::ripple)

enable_testing()
add_test(NAME ripple_package_test COMMAND ripple_package_test)
