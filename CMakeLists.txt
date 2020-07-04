cmake_minimum_required(VERSION 3.16)
project(codepunk)

find_package(LLVM 10 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 20)

file(GLOB TU_LIST src/*.cpp)
add_executable(codepunk ${TU_LIST})

target_compile_definitions(codepunk PRIVATE ${LLVM_DEFINITIONS})
target_include_directories(codepunk PRIVATE ${LLVM_INCLUDE_DIRS} include)

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(codepunk ${llvm_libs})
