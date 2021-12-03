#pragma  once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace voxelizer
{

	struct mesh
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::ivec3> triangles;
	};

	struct material
	{
		glm::vec4 color;
	};

	struct object
	{
		struct mesh mesh;
		struct material material;
	};

	struct scene
	{
		std::vector<object> objects;
	};

}
