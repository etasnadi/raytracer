cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(app VERSION 1.0.0 LANGUAGES CXX CUDA)
set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)    

set(SOURCE_DIR "src")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

set(CUDA_SRCS 
    renderer.cu
    triangle.cc 
    sphere.cc 
    shader.cc 
    room_scene.cc 
    raytracer_basics.cc 
    models.cc
    basic_types.cc 
    math.cc)

list(TRANSFORM CUDA_SRCS PREPEND ${SOURCE_DIR}/)
set_source_files_properties(${CUDA_SRCS} PROPERTIES LANGUAGE CUDA)
add_library(devcode SHARED ${CUDA_SRCS})

add_executable(sdlapp ${SOURCE_DIR}/app.cc ${SOURCE_DIR}/disp_sdl.cc)
target_include_directories(sdlapp PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
target_link_libraries(sdlapp SDL2 SDL2_image devcode)