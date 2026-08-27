cmake_minimum_required(VERSION 3.16)
project(MayaRealtimeLighting CUDA CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Version
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)

# Options
option(BUILD_SHARED_LIBS "Build as shared library" ON)
option(BUILD_TESTS "Build test suite" OFF)
option(ENABLE_CUDA "Enable CUDA support" ON)

# Find required packages
find_package(Maya REQUIRED)
find_package(CUDA REQUIRED)
find_package(OpenGL REQUIRED)

# CUDA settings
set(CUDA_SEPARABLE_COMPILATION ON)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_70")
set(CMAKE_CUDA_FLAGS_DEBUG "-G -g -O0")
set(CMAKE_CUDA_FLAGS_RELEASE "-O3")

# Include directories
include_directories(
    ${MAYA_INCLUDE_DIR}
    ${CUDA_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(PLUGIN_SOURCES
    src/plugin/pluginMain.cpp
    src/plugin/plugin.cpp
    src/viewport/viewportRenderer.cpp
    src/viewport/shader.cpp
    src/core/lightingEngine.cpp
    src/core/scene.cpp
    src/geometry/mesh.cpp
    src/geometry/bvh.cpp
    src/material/material.cpp
    src/utils/logger.cpp
)

set(CUDA_SOURCES
    src/core/raytracer.cu
)

# Compile CUDA files
cuda_compile(CUDA_OBJECTS ${CUDA_SOURCES})

# Create plugin library
add_library(MayaRealtimeLighting SHARED
    ${PLUGIN_SOURCES}
    ${CUDA_OBJECTS}
)

# Link libraries
target_link_libraries(MayaRealtimeLighting
    ${MAYA_LIBRARIES}
    ${CUDA_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

# Set output directory
set_target_properties(MayaRealtimeLighting PROPERTIES
    PREFIX ""
    SUFFIX ".mll"
)

if(WIN32)
    set_target_properties(MayaRealtimeLighting PROPERTIES SUFFIX ".mll")
elseif(APPLE)
    set_target_properties(MayaRealtimeLighting PROPERTIES SUFFIX ".bundle")
else()
    set_target_properties(MayaRealtimeLighting PROPERTIES SUFFIX ".so")
endif()

# Installation
install(TARGETS MayaRealtimeLighting
    LIBRARY DESTINATION plug-ins
    RUNTIME DESTINATION plug-ins
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

message(STATUS "Maya Real-time Lighting Plugin Configuration")
message(STATUS "  Maya Location: ${MAYA_LOCATION}")
message(STATUS "  CUDA Toolkit: ${CUDA_TOOLKIT_ROOT_DIR}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  CUDA Architecture: sm_70+")
