#pragma once

#include <voxelizer/scene.hpp>

#include <filesystem>

namespace voxelizer
{

	scene parse_obj(std::filesystem::path const & path);

}
