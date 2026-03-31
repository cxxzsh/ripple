include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.4.12
    EXCLUDE_FROM_ALL   
)
FetchContent_MakeAvailable(doctest)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

function(ripple_add_test target source)
  add_executable(${target} ${source})
  target_link_libraries(${target} PRIVATE ripple::ripple doctest::doctest)
  target_compile_features(${target} PRIVATE cxx_std_17)
  doctest_discover_tests(${target})
endfunction()

ripple_add_test(test_variable test_variable.cpp)
ripple_add_test(test_derived  test_derived.cpp)

add_custom_target(test_all
  COMMAND test_variable --force-colors=true
  COMMAND test_derived  --force-colors=true
  DEPENDS test_variable test_derived
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running all ripple tests (with color)"
)