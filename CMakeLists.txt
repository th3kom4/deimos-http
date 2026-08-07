cmake_minimum_required(VERSION 3.15)
project(deimos-http CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")

add_executable(server ${SOURCES} ${HEADERS})

target_include_directories(server PRIVATE include)
target_compile_options(server PRIVATE -Wall -Wextra)
