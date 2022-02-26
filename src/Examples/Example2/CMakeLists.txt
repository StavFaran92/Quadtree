set(TARGET_NAME Example2)

# set the project name
project(${TARGET_NAME}
	VERSION 1.0
    DESCRIPTION "Example 2"
    LANGUAGES CXX)

set(SOURCE_FILES main.cpp
	../../Quadtree.h
	../../Utils.h
	../../Utils.cpp)

# add the executable
add_executable(${TARGET_NAME} ${SOURCE_FILES})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(${TARGET_NAME} PRIVATE /W4)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
endif()

target_include_directories(${TARGET_NAME} PRIVATE ${QUADTREE_DIR}/src)

target_include_directories(${TARGET_NAME} PRIVATE ${SFML_DIR}/include)

link_directories(${SFML_DIR}/lib/$<CONFIGURATION>)

target_link_libraries(${TARGET_NAME} PRIVATE sfml-system)
target_link_libraries(${TARGET_NAME} PRIVATE sfml-graphics)
target_link_libraries(${TARGET_NAME} PRIVATE sfml-window)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>
  COMMAND_EXPAND_LISTS)