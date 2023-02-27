# Qt cmake_minimum_required(VERSION 3.1.0)
# OpenMP cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

OPTION(USE_CUDA "Use CUDA (not fully ported yet)" OFF)
if(USE_CUDA)
    project(app VERSION 1.0.0 LANGUAGES CXX CUDA)
    # Suppress GLM warnings
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcudafe --diag_suppress=3057")
    set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)    
else()
    project(app VERSION 1.0.0 LANGUAGES CXX)
endif()

set(SOURCE_DIR "src")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenMP)

add_library(renderer SHARED 
    ${SOURCE_DIR}/renderer.cc
    ${SOURCE_DIR}/shader.cc
    ${SOURCE_DIR}/raytracer_basics.cc
    ${SOURCE_DIR}/triangle.cc
    ${SOURCE_DIR}/sphere.cc
    ${SOURCE_DIR}/models.cc
    ${SOURCE_DIR}/room_scene.cc
    ${SOURCE_DIR}/basic_types.cc
)

add_executable(app
    ${SOURCE_DIR}/app.cc
    ${SOURCE_DIR}/display.cc
    ${SOURCE_DIR}/basic_types.cc
)

if(USE_CUDA)
    set_source_files_properties(renderer.cc raytracer_basics.cc PROPERTIES LANGUAGE CUDA)
endif()

target_link_libraries(renderer OpenMP::OpenMP_CXX)

target_link_libraries(app Qt5::Widgets OpenMP::OpenMP_CXX renderer)