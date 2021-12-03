#pragma once

#include <voxelizer/texture.hpp>
#include <voxelizer/scene.hpp>
#include <voxelizer/bbox.hpp>

#include <functional>

namespace voxelizer
{

	using progress_callback = std::function<void(float, float)>;

	float sample(scene const & input_scene, texture_3d & output_texture, int nsamples,
		box bbox, progress_callback callback = {});

}
