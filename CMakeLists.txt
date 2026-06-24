cmake_minimum_required(VERSION 3.15)
project(David_Goliat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/game.cpp
    src/player.cpp
    src/level1.cpp
    src/level2.cpp
    src/utils.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
