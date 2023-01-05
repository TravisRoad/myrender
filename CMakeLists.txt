cmake_minimum_required(VERSION 3.10)
project(myrender VERSION 0.1)

add_executable(${PROJECT_NAME} main.cpp tgaimage.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
