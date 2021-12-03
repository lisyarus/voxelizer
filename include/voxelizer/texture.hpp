#pragma once

#include <glm/vec3.hpp>

#include <memory>
#include <algorithm>

namespace voxelizer
{

	struct texture_3d
	{
		texture_3d(glm::ivec3 dimensions)
			: dimensions_(dimensions)
			, pixels_(new std::uint32_t[dimensions.x * dimensions.y * dimensions.z])
		{}

		std::uint32_t * data() { return pixels_.get(); }
		std::uint32_t const * data() const { return pixels_.get(); }

		glm::ivec3 dimensions() const { return dimensions_; }

		std::uint32_t & at(int x, int y, int z)
		{
			return pixels_[x + dimensions_.x * (y + dimensions_.y * z)];
		}

		std::uint32_t const & at(int x, int y, int z) const
		{
			return const_cast<texture_3d *>(this)->at(x, y, z);
		}

		void fill(std::uint32_t value)
		{
			std::uint32_t * begin = pixels_.get();
			std::uint32_t * end = begin + dimensions_.x * dimensions_.y * dimensions_.z;
			std::fill(begin, end, value);
		}

	private:
		glm::ivec3 dimensions_;
		std::unique_ptr<std::uint32_t[]> pixels_;
	};

}
