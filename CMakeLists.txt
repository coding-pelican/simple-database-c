cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_BUILD_TYPE Debug)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})

project(${ProjectId}
    VERSION 0.1.0
    DESCRIPTION "My Simple Database Project"
    LANGUAGES C
)

# GLOBING
file(GLOB_RECURSE SOURCE src/*.c)
add_executable(${PROJECT_NAME} main.c ${SOURCE})

target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)