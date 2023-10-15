cmake_minimum_required(VERSION 3.25)

project(Saavi CXX)

# Support only Linux for now
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "Project requires a Linux operating system.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
