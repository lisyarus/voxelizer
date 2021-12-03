#pragma once

#include <glm/vec4.hpp>

#include <cstdint>
#include <cmath>

namespace voxelizer
{

	inline std::uint32_t to_uint(glm::vec4 const & color)
	{
		auto convert = [](float value)
		{
			return static_cast<std::uint32_t>(std::max(0.f, std::min(255.f, value * 255.f)));
		};

		std::uint32_t pixel = 0;

		pixel |= convert(color.r) << 0;
		pixel |= convert(color.g) << 8;
		pixel |= convert(color.b) << 16;
		pixel |= convert(color.a) << 24;

		return pixel;
	}

	inline glm::vec4 to_vec4(std::uint32_t pixel)
	{
		glm::vec4 color;
		color.r = ((pixel & 0x000000ffu) >> 0 ) / 255.f;
		color.g = ((pixel & 0x0000ff00u) >> 8 ) / 255.f;
		color.b = ((pixel & 0x00ff0000u) >> 16) / 255.f;
		color.a = ((pixel & 0xff000000u) >> 24) / 255.f;
		return color;
	}

	inline glm::vec4 premult(glm::vec4 const & c)
	{
		return {c.r * c.a, c.g * c.a, c.b * c.a, c.a};
	}

	inline glm::vec4 unpremult(glm::vec4 const & c)
	{
		if (c.a == 0.f)
			return glm::vec4(0.f);
		return {c.r / c.a, c.g / c.a, c.b / c.a, c.a};
	}

}
