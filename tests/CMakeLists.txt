
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

function(add_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} fmt::fmt)
  target_link_libraries(${name} GTest::gtest_main)
  target_include_directories(${name} PRIVATE ../)

  include(GoogleTest)
  gtest_discover_tests(${name})
  target_precompile_headers(${name}
    PRIVATE
      "<atomic>"
      "<deque>"
      "<fmt/core.h>"
      "<functional>"
      "<future>"
      "<iostream>"
      "<list>"
      "<map>"
      "<memory>"
      "<string>"
      "<thread>"
      "<vector>"
  )
endfunction()

add_test(test_protocol)
add_test(test_address)
add_test(test_bufferbuilder)