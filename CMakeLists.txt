cmake_minimum_required(VERSION 3.9)

add_compile_options(-Wall -Wextra -Werror)
add_compile_options(-std=c++17)
add_compile_options(-Wno-return-type-c-linkage)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project (stuff)
set (stuff_VERSION_MAJOR 0)
set (stuff_VERSION_MINOR 1)

file(GLOB_RECURSE lexsources CONFIGURE_DEPENDS include/parser/*.l)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.l
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tokens_list.sh ${CMAKE_CURRENT_BINARY_DIR}/lexer.l
  DEPENDS ${lexsources}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(FLEX)
FLEX_TARGET(stuff ${CMAKE_CURRENT_BINARY_DIR}/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

file(GLOB_RECURSE stuffsources CONFIGURE_DEPENDS src/*.cpp)

set_source_files_properties(${FLEX_stuff_OUTPUTS} PROPERTIES COMPILE_FLAGS -Wno-sign-compare)

ADD_LIBRARY(stuff SHARED ${stuffsources} ${FLEX_stuff_OUTPUTS})

target_include_directories(stuff PUBLIC include)
target_link_libraries(stuff ${FLEX_LIBRARIES})

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
target_link_libraries(assembler stuff)

file(GLOB_RECURSE runnersources CONFIGURE_DEPENDS runner/*.cpp)
add_executable(runner ${runnersources})
target_include_directories(runner PUBLIC include)
target_link_libraries(runner stuff)

file(GLOB_RECURSE testsources CONFIGURE_DEPENDS test/*.cpp)
add_executable(tests ${testsources})
target_include_directories(tests PUBLIC include ${GTEST_INCLUDE_DIRS})
target_link_libraries(tests gtest_main stuff)
