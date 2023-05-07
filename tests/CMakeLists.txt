
project(cpp_experiments_gtest)

set(CMAKE_CXX_STANDARD 20)
find_package(fmt)
set(Boost_USE_MULTITHREADED      ON)
find_package(Boost REQUIRED COMPONENTS thread)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_protocol test_protocol.cpp)
target_link_libraries(test_protocol fmt::fmt)
target_link_libraries(test_protocol GTest::gtest_main)
target_include_directories(test_protocol PRIVATE ../)

include(GoogleTest)
gtest_discover_tests(test_protocol)
