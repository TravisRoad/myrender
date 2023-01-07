cmake_minimum_required(VERSION 3.10)
project(myrender VERSION 0.1)

add_executable(${PROJECT_NAME} src/main.cpp src/utils/model.cpp)

add_library(tgaimage src/tgaimage.cpp)
target_include_directories(tgaimage PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC tgaimage)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/utils)
