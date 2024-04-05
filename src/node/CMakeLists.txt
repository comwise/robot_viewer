cmake_minimum_required(VERSION 3.2)

include_directories(node)

file(GLOB_RECURSE SOURCE_FILES "*.c*")

#add_library(robot_viewer ${SOURCE_FILES})
#target_link_libraries(robot_viewer pthread rt)

add_executable(robot_viewer main.cpp)
target_link_libraries(robot_viewer map jsoncpp)