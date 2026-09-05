cmake_minimum_required(VERSION 3.16)

project(CPStream VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external
)

add_executable(cpstream
    src/main.cpp
    src/ui.cpp
    src/problem.cpp
    src/server.cpp
    src/commands.cpp
    src/showHelp.cpp
)

if(WIN32)
    target_link_libraries(cpstream ws2_32)
endif()