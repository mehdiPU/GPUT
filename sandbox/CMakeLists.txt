cmake_minimum_required(VERSION 3.10)

project(sandbox)

add_executable(${PROJECT_NAME} src/sandbox.c)

target_link_libraries(${PROJECT_NAME} PRIVATE gput)
