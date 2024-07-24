cmake_minimum_required(VERSION 3.20)
project(tg-bot)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cc)

add_executable(tg-bot ${SOURCES})

find_package(tgbot REQUIRED)
target_link_libraries(tg-bot tgbot::tgbot)