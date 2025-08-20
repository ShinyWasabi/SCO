cmake_minimum_required(VERSION 3.20)

project(SCOL CXX)

include(cmake/async-logger.cmake)
include(cmake/minhook.cmake)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/**.hpp"
    "${SRC_DIR}/**.cpp"
)

add_library(${PROJECT_NAME} MODULE ${SRC_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_precompile_headers(${PROJECT_NAME} PUBLIC "${SRC_DIR}/Common.hpp")

add_compile_definitions(
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${SRC_DIR}"
    "${minhook_SOURCE_DIR}/include"
    "${minhook_SOURCE_DIR}/src/hde"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    AsyncLogger
    minhook
)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".asi")