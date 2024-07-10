cmake_minimum_required(VERSION 3.15)

project(BombField VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)

set(SRC_FILES
	src/Bombfield.cpp
	src/Grid.cpp
	src/Menu.cpp
	)

add_executable(BombField)

configure_file(${CMAKE_SOURCE_DIR}/include/BF_version.h.in ${CMAKE_SOURCE_DIR}/include/BF_version.h)

target_sources(BombField
	PRIVATE ${SRC_FILES}
	)

target_include_directories(BombField 
	PUBLIC ${CMAKE_SOURCE_DIR}/include
	)

add_custom_command(TARGET BombField POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:BombField>/data
	)
