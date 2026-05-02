cmake_minimum_required(VERSION 4.0)
project(Pirahna)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(Pirahna camTest.cpp)

target_link_libraries(Pirahna ${OpenCV_LIBS})
