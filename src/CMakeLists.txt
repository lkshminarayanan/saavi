# generate the shared library
add_library(saavi SHARED saavi.cpp file_iterators.cpp)

# generate the CLI tool
add_executable(saavi-cli cli.cpp)
target_link_libraries(saavi-cli saavi)
