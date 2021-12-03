#include <voxelizer/bbox.hpp>

#include <limits>

namespace voxelizer
{

	box bbox(scene const & s)
	{
		box result;

		for (auto const & o : s.objects)
		{
			for (auto const & v : o.mesh.vertices)
			{
				result.append(v);
			}
		}

		return result;
	}

}
