option(WITH_TESTS "Enable all the tests" ON)

if(NOT WITH_TESTS)
  message(STATUS "Skip building tests")
  return()
endif()

# Add google test as a dependency
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main # use the commit at head
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# Read all files ending with _test.cpp and compile them into a google test
include_directories(${Saavi_SOURCE_DIR}/src)
file(
  GLOB TESTFILES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*_test.cpp")
add_executable(saaviTests test_main.cpp ${TESTFILES})
target_link_libraries(saaviTests GTest::gtest_main saavi)
gtest_discover_tests(saaviTests)
