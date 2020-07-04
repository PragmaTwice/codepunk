cmake_minimum_required(VERSION 3.16)

configure_file(googletest/CMakeLists.txt.in googletest-download/CMakeLists.txt)
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

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

project(codepunk)

find_package(LLVM 10 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 20)

file(GLOB TU_LIST src/*.cpp)
file(GLOB TEST_LIST test/*.cpp)

llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(codepunk ${TU_LIST})

target_compile_definitions(codepunk PRIVATE ${LLVM_DEFINITIONS})
target_include_directories(codepunk PRIVATE ${LLVM_INCLUDE_DIRS} include)
target_link_libraries(codepunk ${llvm_libs})

add_executable(codepunk_test ${TEST_LIST})

target_compile_definitions(codepunk_test PRIVATE ${LLVM_DEFINITIONS})
target_include_directories(codepunk_test PRIVATE ${LLVM_INCLUDE_DIRS} include)
target_link_libraries(codepunk_test ${llvm_libs} gtest_main)

enable_testing()
add_test(NAME test COMMAND codepunk_test)
