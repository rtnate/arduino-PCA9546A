cmake_minimum_required(VERSION 3.23)

project(Arduino-Library-PCA9546A LANGUAGES C CXX ASM)

add_library(arduino_PCA9546A STATIC
  ${CMAKE_CURRENT_LIST_DIR}/src/PCA9546A.cpp
)

target_include_directories(arduino_PCA9546A
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/src
)