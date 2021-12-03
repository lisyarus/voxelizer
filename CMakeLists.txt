cmake_minimum_required(VERSION 3.0)
project(voxelizer)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

add_subdirectory(glm)

set(TARGET_NAME "${PROJECT_NAME}")

add_executable(${TARGET_NAME}
	source/main.cpp
	source/obj.cpp
	source/bbox.cpp
	source/index.cpp
	source/sampler.cpp
	source/flood_fill.cpp
	include/voxelizer/scene.hpp
	include/voxelizer/obj.hpp
	include/voxelizer/bbox.hpp
	include/voxelizer/color.hpp
	include/voxelizer/texture.hpp
	include/voxelizer/sequence.hpp
	include/voxelizer/index.hpp
	include/voxelizer/sampler.hpp
	include/voxelizer/flood_fill.hpp
)
target_include_directories(${TARGET_NAME} PUBLIC
	include
)
target_link_libraries(${TARGET_NAME} PUBLIC
	glm
	Threads::Threads
)
