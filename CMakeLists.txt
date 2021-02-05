cmake_minimum_required(VERSION 3.10)

project(nauka)

add_executable(nauka main.c)
find_package(Threads REQUIRED)
target_link_libraries(nauka Threads::Threads)