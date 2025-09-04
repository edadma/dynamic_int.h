cmake_minimum_required(VERSION 3.10)

project(dynamic_int 
    VERSION 1.1.0
    DESCRIPTION "Reference-counted arbitrary precision integer library"
    LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Header-only library target
add_library(dynamic_int INTERFACE)
target_include_directories(dynamic_int INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Unity test framework
add_library(unity STATIC
    libs/unity/unity.c
)
target_include_directories(unity PUBLIC libs/unity)

# Test executable
add_executable(tests
    main.c
)
target_link_libraries(tests PRIVATE dynamic_int unity m)
target_compile_definitions(tests PRIVATE DI_IMPLEMENTATION)

# Enable testing
enable_testing()
add_test(NAME dynamic_int_tests COMMAND tests)

# Install configuration
install(FILES dynamic_int.h
    DESTINATION include
)

install(TARGETS dynamic_int
    EXPORT dynamic_intTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(EXPORT dynamic_intTargets
    FILE dynamic_intTargets.cmake
    NAMESPACE dynamic_int::
    DESTINATION lib/cmake/dynamic_int
)

# Create config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    dynamic_intConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/dynamic_intConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/dynamic_intConfig.cmake
    INSTALL_DESTINATION lib/cmake/dynamic_int
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/dynamic_intConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/dynamic_intConfigVersion.cmake
    DESTINATION lib/cmake/dynamic_int
)