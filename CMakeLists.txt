cmake_minimum_required(VERSION 3.16)
project(ripple VERSION 0.1.0 LANGUAGES CXX)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/ripple/version.hpp
    @ONLY
)

add_library(ripple INTERFACE)
add_library(ripple::ripple ALIAS ripple)

target_compile_features(ripple INTERFACE cxx_std_17)

target_include_directories(ripple INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(RIPPLE_IS_TOP_LEVEL ON)
else()
    set(RIPPLE_IS_TOP_LEVEL OFF)
endif()

option(RIPPLE_BUILD_TESTS "Build tests" ${RIPPLE_IS_TOP_LEVEL})

if(RIPPLE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# -------------------- install --------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY include/ripple
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/include/ripple/version.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ripple
)

install(TARGETS ripple
    EXPORT rippleTargets
)

install(EXPORT rippleTargets
    FILE        rippleTargets.cmake
    NAMESPACE   ripple::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ripple
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/rippleConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/rippleConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ripple
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/rippleConfigVersion.cmake
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/rippleConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/rippleConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ripple
)
