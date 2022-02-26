cmake_minimum_required(VERSION 3.10)

# set the project name
project(Quadtree
	VERSION 1.0
    DESCRIPTION "A CPP lightweight implementation of a Quadtree"
    LANGUAGES CXX)

set(QUADTREE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Setup SFML
set(SFML_DIR extern/SFML)
set(SFML_BUILD_WINDOW TRUE)
set(SFML_BUILD_GRAPHICS TRUE)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
set(SFML_BUILD_DOC FALSE)
set(SFML_BUILD_EXAMPLES FALSE)

#add SFML
add_subdirectory(${SFML_DIR})

#add examples
add_subdirectory(src/Examples/Example1)
add_subdirectory(src/Examples/Example2)