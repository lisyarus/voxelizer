#include <voxelizer/index.hpp>

#include <glm/mat3x3.hpp>

#include <limits>

namespace voxelizer
{

	bool intersects(glm::vec3 const & origin, glm::vec3 const & direction, box const & b)
	{
		static constexpr float inf = std::numeric_limits<float>::infinity();

		float tmin = -inf;
		float tmax = inf;

		for (int d = 0; d < 3; ++d)
		{
			if (direction[d] == 0.f)
			{
				if (origin[d] < b.min[d] || origin[d] > b.max[d])
					return false;
				continue;
			}

			float t1 = (b.min[d] - origin[d]) / direction[d];
			float t2 = (b.max[d] - origin[d]) / direction[d];
			if (t1 > t2)
				std::swap(t1, t2);

			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);
		}

		if (tmin > tmax)
			return false;

		if (tmax < 0.f)
			return false;

		return true;
	}

	bool intersects(glm::vec3 const & origin, glm::vec3 const & direction, glm::vec3 const & p0, glm::vec3 const & p1, glm::vec3 const & p2)
	{
		glm::mat3 g;
		g[0] = direction;
		g[1] = p0 - p1;
		g[2] = p0 - p2;

		auto solution = glm::inverse(g) * (p0 - origin);

		return solution[0] >= 0.f && solution[1] >= 0.f && solution[2] >= 0.f && solution[1] + solution[2] <= 1.f;
	}

	index::index(mesh const & m)
		: mesh_(m)
	{
		box full_bbox;

		std::vector<triangle_data> triangles;
		for (std::uint32_t i = 0; i < mesh_.triangles.size(); ++i)
		{
			triangle_data data;
			data.id = i;

			auto const & t = mesh_.triangles[i];

			for (int v = 0; v < 3; ++v)
			{
				auto const & p = mesh_.vertices[t[v]];
				data.bbox.append(p);
				full_bbox.append(p);
			}

			triangles.push_back(data);
		}

		build_tree(std::move(triangles), full_bbox, 0);
	}

	index::node_id index::build_tree(std::vector<triangle_data> triangles, box bbox, int type)
	{
		float middle = (bbox.min[type] + bbox.max[type]) * 0.5f;

		auto result = create_node(bbox);

		auto left_end = std::partition(triangles.begin(), triangles.end(), [middle, type](triangle_data const & data){ return data.bbox.max[type] <= middle; });
		auto middle_begin = std::partition(left_end, triangles.end(), [middle, type](triangle_data const & data){ return data.bbox.min[type] <= middle; });

		if (left_end == triangles.begin())
		{
			nodes_[result].triangles = std::move(triangles);
		}
		else if (middle_begin == triangles.end())
		{
			nodes_[result].triangles = std::move(triangles);
		}
		else
		{
			auto divide = left_end;

			box left_bbox;
			for (auto it = triangles.begin(); it != divide; ++it)
				left_bbox.append(it->bbox);

			box right_bbox;
			for (auto it = divide; it != triangles.end(); ++it)
				right_bbox.append(it->bbox);

			auto left = build_tree({triangles.begin(), divide}, left_bbox, (type + 1) % 3);
			auto right = build_tree({divide, triangles.end()}, right_bbox, (type + 1) % 3);

			nodes_[result].left = left;
			nodes_[result].right = right;
		}

		return result;
	}

}
