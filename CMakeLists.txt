cmake_minimum_required(VERSION 3.10)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /std:c++latest /W3")

project(MKXAIProject VERSION 0.1.0)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
#file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/src/*.h")

set(_src_root_path "${CMAKE_SOURCE_DIR}/src")
file(
    GLOB_RECURSE _source_list
    LIST_DIRECTORIES false
    "${_src_root_path}/*.cpp*"
    "${_src_root_path}/*.h*"
)

# Set include and link directories variables
set(PYTHON_INCLUDE "D:/Anaconda/include")
set(VIGEM_INCLUDE "D:/repos/vigem/ViGEmUM")
set(VIGEM_COMMON_INCLUDE "D:/repos/vigem/Common")
set(NUMPY_INCLUDE "D:/Anaconda/Lib/site-packages/numpy/core/include/numpy")
set(PYTHON_LIBS "D:/Anaconda/pkgs/python-3.5.2-0/libs")
set(VIGEM_LIBS "D:/repos/vigem/x64")

include_directories(${CMAKE_SOURCE_DIR}/src/General ${CMAKE_SOURCE_DIR}/src/Player ${CMAKE_SOURCE_DIR}/src/Utility ${PYTHON_INCLUDE} ${CMAKE_SOURCE_DIR}/contrib ${VIGEM_INCLUDE} ${VIGEM_COMMON_INCLUDE} $ENV{OPENCV_DIR}/../../include ${NUMPY_INCLUDE})
link_directories(${PYTHON_LIBS} ${VIGEM_LIBS} $ENV{OPENCV_DIR}/lib )

add_executable(Engine ${_source_list})

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

target_link_libraries(Engine general opencv_world320.lib ViGEmUM.lib)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Engine)
