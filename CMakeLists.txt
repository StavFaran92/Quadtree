cmake_minimum_required(VERSION 3.10)

# set the project name
project(Quadtree
	VERSION 1.0
    DESCRIPTION "Quadtree CPP lightweight implementation"
    LANGUAGES CXX)

set(SOURCE_FILES 
	src/Quadtree.h
	src/Utils.h
	src/Utils.cpp)

# add the executable
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Quadtree)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SFML_DIR extern/SFML)

set(SFML_BUILD_WINDOW TRUE)
set(SFML_BUILD_GRAPHICS TRUE)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
set(SFML_BUILD_DOC FALSE)
set(SFML_BUILD_EXAMPLES FALSE)

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
endif()

add_subdirectory(${SFML_DIR})

#add_subdirectory(src/Examples/Example1)