cmake_minimum_required(VERSION 3.24)

project(cpp_experiments)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*")
find_package(fmt)

set(Boost_USE_MULTITHREADED      ON)
find_package(Boost REQUIRED COMPONENTS thread)

function(add_experiment name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} fmt::fmt)
    # set_target_properties(${name} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endfunction()

function(add_boost_experiment name)
    add_experiment(${name})
    if(Boost_FOUND)
        target_include_directories(${name} PRIVATE ${Boost_INCLUDE_DIRS})
        target_link_libraries(${name} ${Boost_LIBRARIES})
        target_precompile_headers(${name}
            PRIVATE
                "<boost/asio.hpp>"
                "<boost/array.hpp>"
                "<boost/bind.hpp>"
                "<boost/thread.hpp>"
        )
    endif()  
endfunction()

add_boost_experiment(boost_serial)
add_experiment(buffer)
add_experiment(fmt)
add_experiment(idgen)
add_experiment(main_loop)
add_experiment(thread_pool)
add_experiment(tokenize_string)