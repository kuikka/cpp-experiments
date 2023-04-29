cmake_minimum_required(VERSION 3.24)

project(cpp_experiments)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*")
find_package(fmt)

function(add_experiment name)
add_executable(${name} ${name}.cpp)
target_link_libraries(${name} fmt::fmt)
set_target_properties(${name} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endfunction()

add_experiment(buffer)
add_experiment(fmt)
add_experiment(main_loop)
add_experiment(thread_pool)
add_experiment(tokenize_string)