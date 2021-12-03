#pragma once

#include <glm/vec3.hpp>

#include <cmath>

namespace voxelizer
{

	// R_3 quazirandom sequence, see http://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences
	struct sequence
	{
		sequence(glm::vec3 start)
			: value_(start)
		{}

		glm::vec3 operator() ()
		{
			auto tmp = value_;
			value_ += glm::vec3{1.f / 1.61803398874989f, 1.f / 1.32471795724475f, 1.f / 1.22074408460576f};
			for (int i : {0, 1, 2}) value_[i] = std::fmod(value_[i], 1.f);
			return tmp;
		}

	private:
		glm::vec3 value_;
	};

}
