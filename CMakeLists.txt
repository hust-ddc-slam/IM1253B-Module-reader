
project(current_reader)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)


include_directories(
    include
    /usr/local/include
)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/IM1253B.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    boost_system
    pthread
)
