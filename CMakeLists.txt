# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
SET(CMAKE_CXX_FLAGS "-std=c++11")

project (FirstProject)

FIND_PACKAGE(Vigra)
set(SOURCES utils/Point.cpp algorithms/valleyLocalization.cpp main.cpp)
set(HEADERS utils/Point.h algorithms/valleyLocalization.h)
add_executable (main main.cpp)
TARGET_INCLUDE_DIRECTORIES(main PUBLIC ${Vigra_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(main vigraimpex)
