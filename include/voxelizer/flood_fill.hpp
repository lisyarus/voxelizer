#pragma once

#include <voxelizer/texture.hpp>

#include <functional>

namespace voxelizer
{

	using progress_callback = std::function<void(float, float)>;

	void flood_fill(texture_3d & texture, progress_callback callback = {});

}
