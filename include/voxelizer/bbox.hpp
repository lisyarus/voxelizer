#pragma once

#include <voxelizer/scene.hpp>

#include <limits>

namespace voxelizer
{

	struct box
	{
		static constexpr float inf = std::numeric_limits<float>::infinity();

		glm::vec3 min = glm::vec3(inf);
		glm::vec3 max = glm::vec3(-inf);

		void append(glm::vec3 const & p)
		{
			for (int i : {0, 1, 2})
			{
				min[i] = std::min(min[i], p[i]);
				max[i] = std::max(max[i], p[i]);
			}
		}

		void append(box const & b)
		{
			for (int i : {0, 1, 2})
			{
				min[i] = std::min(min[i], b.min[i]);
				max[i] = std::max(max[i], b.max[i]);
			}
		}
	};

	box bbox(scene const & s);

}
