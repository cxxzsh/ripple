include(FetchContent)

set(DOCTEST_NO_INSTALL ON CACHE BOOL "Skip doctest installation" FORCE)

FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG v2.4.12
)
FetchContent_MakeAvailable(doctest)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

function(ripple_add_test target source)
  add_executable(${target} ${source})
  target_link_libraries(${target} PRIVATE ripple::ripple doctest::doctest)
  target_compile_features(${target} PRIVATE cxx_std_17)
  doctest_discover_tests(${target})
endfunction()

ripple_add_test(ripple_var_test var_test.cpp)
ripple_add_test(ripple_computed_test computed_test.cpp)
ripple_add_test(ripple_effect_test effect_test.cpp)
ripple_add_test(ripple_batch_test batch_test.cpp)

add_custom_target(test_all
  COMMAND ripple_var_test --force-colors=true
  COMMAND ripple_computed_test --force-colors=true
  COMMAND ripple_effect_test --force-colors=true
  COMMAND ripple_batch_test --force-colors=true
  DEPENDS ripple_var_test ripple_computed_test ripple_effect_test ripple_batch_test
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running all ripple tests (with color)"
)
