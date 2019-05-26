cmake_minimum_required(VERSION 3.9)

add_compile_options(-Wall -Wextra -Werror)
add_compile_options(-std=c++17)
add_compile_options(-Wno-return-type-c-linkage)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project (krakatau)
set (krakatau_VERSION_MAJOR 0)
set (krakatau_VERSION_MINOR 1)

find_package(FLEX)

file(GLOB_RECURSE lexsources CONFIGURE_DEPENDS include/parser/*.l)
FLEX_TARGET(lexer ${lexsources} ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
set_source_files_properties(${FLEX_lexer_OUTPUTS} PROPERTIES COMPILE_FLAGS -Wno-sign-compare)

file(GLOB_RECURSE coresources CONFIGURE_DEPENDS src/*.cpp)
ADD_LIBRARY(core SHARED ${coresources} ${FLEX_lexer_OUTPUTS})

target_include_directories(core PUBLIC include)
target_link_libraries(core ${FLEX_LIBRARIES})
target_link_libraries(core dl)

# Download and unpack googletest at configure time
configure_file(CMakeLists.gtest.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

file(GLOB_RECURSE assemblersources CONFIGURE_DEPENDS assembler/*.cpp)
add_executable(assembler ${assemblersources})
target_include_directories(assembler PUBLIC include)
target_link_libraries(assembler core)

file(GLOB_RECURSE runnersources CONFIGURE_DEPENDS runner/*.cpp)
add_executable(runner ${runnersources})
target_include_directories(runner PUBLIC include)
target_link_libraries(runner core)

file(GLOB_RECURSE testsources CONFIGURE_DEPENDS test/*.cpp)
add_executable(tests ${testsources})
target_include_directories(tests PUBLIC include ${GTEST_INCLUDE_DIRS})
target_link_libraries(tests gtest_main core)

file(GLOB_RECURSE native_time_sources CONFIGURE_DEPENDS native/src/time/*.cpp)
add_library(native_time SHARED ${native_time_sources})
target_include_directories(native_time PUBLIC include native/include)
target_link_libraries(native_time core)
